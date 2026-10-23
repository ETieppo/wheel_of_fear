#include "core/audio/OggOpusDecoder.hpp"

#include <algorithm>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <string_view>

#include <opus.h>

#include "core/assets/AssetLocator.hpp"

namespace wheel_of_fear {

namespace {

constexpr std::size_t page_header_size = 27;
constexpr int opus_sample_rate = 48000;
constexpr int maximum_frame_samples = 5760;

struct OpusDecoderDeleter {
    void operator()(OpusDecoder* decoder) const { opus_decoder_destroy(decoder); }
};

std::uint16_t readLittleEndian16(const std::uint8_t* data) {
    return static_cast<std::uint16_t>(data[0] | (data[1] << 8));
}

std::int64_t readLittleEndian64(const std::uint8_t* data) {
    std::uint64_t value = 0;
    for (int index = 7; index >= 0; --index) value = (value << 8) | data[index];
    return static_cast<std::int64_t>(value);
}

bool startsWith(const std::vector<std::uint8_t>& packet, std::string_view magic) {
    return packet.size() >= magic.size() && std::memcmp(packet.data(), magic.data(), magic.size()) == 0;
}

}

DecodedAudioProps OggOpusDecoder::decodeFile(const std::filesystem::path& path) {
    const std::vector<std::uint8_t> bytes = AssetLocator::readBytes(path);
    return decode(bytes);
}

std::vector<std::vector<std::uint8_t>> OggOpusDecoder::extractPackets(std::span<const std::uint8_t> bytes, std::int64_t& final_granule_position) {
    std::vector<std::vector<std::uint8_t>> packets;
    std::vector<std::uint8_t> pending;
    std::size_t offset = 0;
    while (offset + page_header_size <= bytes.size()) {
        if (std::memcmp(bytes.data() + offset, "OggS", 4) != 0) throw std::runtime_error("malformed ogg page");
        const std::int64_t granule_position = readLittleEndian64(bytes.data() + offset + 6);
        const std::size_t segment_count = bytes[offset + 26];
        const std::size_t table_offset = offset + page_header_size;
        if (table_offset + segment_count > bytes.size()) throw std::runtime_error("truncated ogg segment table");
        std::size_t data_offset = table_offset + segment_count;
        for (std::size_t segment = 0; segment < segment_count; ++segment) {
            const std::size_t lacing = bytes[table_offset + segment];
            if (data_offset + lacing > bytes.size()) throw std::runtime_error("truncated ogg packet");
            pending.insert(pending.end(), bytes.begin() + static_cast<std::ptrdiff_t>(data_offset), bytes.begin() + static_cast<std::ptrdiff_t>(data_offset + lacing));
            data_offset += lacing;
            if (lacing < 255) {
                packets.push_back(std::move(pending));
                pending.clear();
            }
        }
        if (granule_position >= 0) final_granule_position = granule_position;
        offset = data_offset;
    }
    return packets;
}

DecodedAudioProps OggOpusDecoder::decode(std::span<const std::uint8_t> bytes) {
    std::int64_t final_granule_position = -1;
    const auto packets = extractPackets(bytes, final_granule_position);
    if (packets.size() < 2 || !startsWith(packets[0], "OpusHead")) throw std::runtime_error("missing opus header");
    const std::vector<std::uint8_t>& head = packets[0];
    if (head.size() < 19) throw std::runtime_error("short opus header");
    const int channels = head[9];
    const std::size_t pre_skip = readLittleEndian16(head.data() + 10);
    if (channels < 1 || channels > 2 || head[18] != 0) throw std::runtime_error("unsupported opus channel mapping");

    int error = 0;
    const std::unique_ptr<OpusDecoder, OpusDecoderDeleter> decoder(opus_decoder_create(opus_sample_rate, channels, &error));
    if (error != OPUS_OK) throw std::runtime_error("unable to create the opus decoder");

    DecodedAudioProps audio;
    audio.channels = static_cast<std::uint32_t>(channels);
    audio.sample_rate = opus_sample_rate;
    std::vector<float> frame(static_cast<std::size_t>(maximum_frame_samples) * static_cast<std::size_t>(channels));
    for (std::size_t index = 1; index < packets.size(); ++index) {
        const std::vector<std::uint8_t>& packet = packets[index];
        if (startsWith(packet, "OpusTags") || packet.empty()) continue;
        const int decoded = opus_decode_float(decoder.get(), packet.data(), static_cast<opus_int32>(packet.size()), frame.data(), maximum_frame_samples, 0);
        if (decoded < 0) throw std::runtime_error("corrupt opus packet");
        audio.samples.insert(audio.samples.end(), frame.begin(), frame.begin() + static_cast<std::ptrdiff_t>(decoded) * channels);
    }

    const std::size_t channel_count = audio.channels;
    std::size_t frame_count = audio.samples.size() / channel_count;
    const std::size_t skipped = std::min(pre_skip, frame_count);
    audio.samples.erase(audio.samples.begin(), audio.samples.begin() + static_cast<std::ptrdiff_t>(skipped * channel_count));
    frame_count -= skipped;
    if (final_granule_position > static_cast<std::int64_t>(pre_skip)) {
        const auto playable_frames = static_cast<std::size_t>(final_granule_position) - pre_skip;
        if (playable_frames < frame_count) audio.samples.resize(playable_frames * channel_count);
    }
    return audio;
}

}
