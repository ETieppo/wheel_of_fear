#pragma once

#include <cstdint>
#include <cstdlib>
#include <span>
#include <string>

#include "game/GameSimulation.hpp"

namespace wheel_of_fear {
  class DemoModeEnvironmentGuard {
    public:
      explicit DemoModeEnvironmentGuard(
        const std::string &demo_mode
      ) {
        setenv("ENGRENAGEM_DEMO", demo_mode.c_str(), 1);
      }

      DemoModeEnvironmentGuard(
        const DemoModeEnvironmentGuard &
      ) = delete;
      DemoModeEnvironmentGuard &operator =(
        const DemoModeEnvironmentGuard &
      ) = delete;

      ~DemoModeEnvironmentGuard() {
        unsetenv("ENGRENAGEM_DEMO");
      }
  };

  class HeadlessGameHarness {
    public:
      static constexpr float frame_seconds = 1.0F / 60.0F;

      explicit HeadlessGameHarness(
        const std::string &demo_mode
      )
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

      void advanceFrames(
        int frame_count
      ) {
        for(int frame = 0; frame < frame_count; ++frame) runFrame();
      }

      [[nodiscard]]
      GameContextDto &context() {
        return simulation_.context();
      }

      [[nodiscard]]
      const FrameProps &frame() const {
        return simulation_.renderer().frame();
      }

    private:
      static constexpr std::uint64_t fnv_offset_basis = 0xCBF29CE484222325ULL;
      static constexpr std::uint64_t fnv_prime        = 0x100000001B3ULL;

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

      template<typename ValueType> static std::uint64_t mixSpan(
        std::uint64_t              hash,
        std::span<const ValueType> values
      ) {
        return mixBytes(hash, std::as_bytes(values));
      }

      void runFrame() {
        simulation_.input().beginFrame();
        simulation_.step(frame_seconds);
      }

      [[nodiscard]]
      std::uint64_t mixFrame(
        std::uint64_t hash
      ) const {
        const FrameProps &current = frame();

        hash = mixSpan(hash, current.world.used());
        hash = mixSpan(hash, current.glow.used());
        hash = mixSpan(hash, current.ui.used());

        hash =
          mixSpan(hash, std::span(current.lights).first(current.light_count));

        hash = mixBytes(hash, std::as_bytes(std::span(&current.post, 1)));
        hash = mixSpan(hash, std::span(current.map_texels));

        return hash;
      }

      DemoModeEnvironmentGuard demo_mode_guard_;
      GameSimulation           simulation_;
  };
} // namespace wheel_of_fear
