#include "support/HeadlessGameHarness.hpp"
#include <array>
#include <cstdint>
#include <doctest/doctest.h>
#include <memory>
#include <string_view>

namespace wheel_of_fear {
  namespace {
    constexpr int frame_count     = 1200;
    constexpr int sample_interval = 15;

    struct GoldenMasterCaseProps {
        std::string_view demo_mode;
        std::uint64_t    expected_fingerprint;
    };

    constexpr GoldenMasterCaseProps goldenCase(
      std::string_view demo_mode,
      std::uint64_t    expected_fingerprint
    ) {
      return GoldenMasterCaseProps {
        .demo_mode            = demo_mode,
        .expected_fingerprint = expected_fingerprint,
      };
    }

    constexpr std::array<GoldenMasterCaseProps, 16> golden_master_cases {
      goldenCase("none", 4918380489932145272ULL),
      goldenCase("play", 5210700163412478118ULL),
      goldenCase("deep", 2524870939948402191ULL),
      goldenCase("tracker", 1315644311588174912ULL),
      goldenCase("sectors", 5288150336535492333ULL),
      goldenCase("showcase", 2440873158419509755ULL),
      goldenCase("freeze", 17125811141788460805ULL),
      goldenCase("moth", 10186916260382730725ULL),
      goldenCase("locker", 16907729882483859861ULL),
      goldenCase("witness", 6652067478933736456ULL),
      goldenCase("repair", 7926940296944293375ULL),
      goldenCase("power", 4326322000128176400ULL),
      goldenCase("grenade", 6586979219360108824ULL),
      goldenCase("lure", 18297758343076773701ULL),
      goldenCase("doors", 4586389178548821122ULL),
      goldenCase("walk", 16523349908505656813ULL),
    };

    std::uint64_t fingerprintOf(std::string_view demo_mode) {
      const auto harness
        = std::make_unique<HeadlessGameHarness>(std::string(demo_mode));
      return harness->runFrames(frame_count, sample_interval);
    }
  } // namespace

  TEST_CASE("headless simulation is deterministic across runs") {
    CHECK(fingerprintOf("play") == fingerprintOf("play"));
  }

  TEST_CASE("every demo scenario renders the recorded golden master") {
    for(const GoldenMasterCaseProps &golden_case : golden_master_cases) {
      CAPTURE(golden_case.demo_mode);
      CHECK(
        fingerprintOf(golden_case.demo_mode) == golden_case.expected_fingerprint
      );
    }
  }
} // namespace wheel_of_fear
