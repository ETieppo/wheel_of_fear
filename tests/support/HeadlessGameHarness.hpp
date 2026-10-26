#pragma once

#include "game/GameSimulation.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <span>
#include <string>
#include <type_traits>

namespace wheel_of_fear {
  class DemoModeEnvironmentGuard {
    public:
      explicit DemoModeEnvironmentGuard(const std::string &demo_mode) {
        setenv("ENGRENAGEM_DEMO", demo_mode.c_str(), 1);
      }

      DemoModeEnvironmentGuard(const DemoModeEnvironmentGuard &) = delete;
      DemoModeEnvironmentGuard &operator =(const DemoModeEnvironmentGuard &)
        = delete;

      ~DemoModeEnvironmentGuard() {
        unsetenv("ENGRENAGEM_DEMO");
      }
  };

  class HeadlessGameHarness {
    public:
      static constexpr float frame_seconds = 1.0F / 60.0F;

      explicit HeadlessGameHarness(const std::string &demo_mode)
          : demo_mode_guard_(demo_mode) {
      }

      std::uint64_t runFrames(
        int frame_count,
        int sample_interval
      ) {
        std::uint64_t fingerprint = fnv_offset_basis;

        for(int frame = 0; frame < frame_count; ++frame) {
          runFrame();
          if(frame % sample_interval == 0) fingerprint = mixFrame(fingerprint);
        }

        return fingerprint;
      }

      void advanceFrames(int frame_count) {
        for(int frame = 0; frame < frame_count; ++frame) runFrame();
      }

      void advanceFramePressing(InputActionEnum action) {
        simulation_.input().beginFrame();
        simulation_.input().simulatePress(action);
        simulation_.step(frame_seconds);
      }

      [[nodiscard]]
      GameContextDto &context() {
        return simulation_.context();
      }

      [[nodiscard]]
      Renderer &renderer() {
        return simulation_.renderer();
      }

      [[nodiscard]]
      const FrameProps &frame() const {
        return simulation_.renderer().frame();
      }

    private:
      static constexpr std::uint64_t fnv_offset_basis = 0xCBF29CE484222325ULL;
      static constexpr std::uint64_t fnv_prime        = 0x100000001B3ULL;
      static constexpr float fingerprint_float_resolution = 1.0F / 64.0F;

      static std::uint64_t mixBytes(
        std::uint64_t              hash,
        std::span<const std::byte> bytes
      ) {
        for(const std::byte value : bytes) {
          hash ^= static_cast<std::uint64_t>(value);
          hash *= fnv_prime;
        }

        return hash;
      }

      static std::uint64_t mixQuantizedFloat(
        std::uint64_t hash,
        float         value
      ) {
        const auto quantized = static_cast<std::int64_t>(
          std::lround(value / fingerprint_float_resolution)
        );

        return mixBytes(hash, std::as_bytes(std::span(&quantized, 1)));
      }

      template<typename FloatRecordType> static std::uint64_t mixFloatRecords(
        std::uint64_t                    hash,
        std::span<const FloatRecordType> records
      ) {
        static_assert(std::is_trivially_copyable_v<FloatRecordType>);
        static_assert(sizeof(FloatRecordType) % sizeof(float) == 0);
        constexpr std::size_t float_count
          = sizeof(FloatRecordType) / sizeof(float);

        for(const FloatRecordType &record : records) {
          const auto fields
            = std::bit_cast<std::array<float, float_count>>(record);

          for(const float field : fields) hash = mixQuantizedFloat(hash, field);
        }

        return hash;
      }

      void runFrame() {
        simulation_.input().beginFrame();
        simulation_.step(frame_seconds);
      }

      [[nodiscard]]
      std::uint64_t mixFrame(std::uint64_t hash) const {
        const FrameProps &current = frame();

        hash = mixFloatRecords(hash, current.world.used());
        hash = mixFloatRecords(hash, current.glow.used());
        hash = mixFloatRecords(hash, current.ui.used());
        hash = mixFloatRecords(
          hash,
          std::span(current.lights).first(current.light_count)
        );
        hash = mixFloatRecords(hash, std::span(&current.post, 1));
        hash = mixBytes(hash, std::as_bytes(std::span(current.map_texels)));

        return hash;
      }

      DemoModeEnvironmentGuard demo_mode_guard_;
      GameSimulation           simulation_ {UserDataFilesProps {}};
  };
} // namespace wheel_of_fear
