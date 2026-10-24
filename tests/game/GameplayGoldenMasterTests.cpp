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
      goldenCase("none", 2260802571275938495ULL),
      goldenCase("play", 4606756502277195433ULL),
      goldenCase("deep", 10354815022693644597ULL),
      goldenCase("tracker", 6212961513808940026ULL),
      goldenCase("sectors", 1834963208022850004ULL),
      goldenCase("showcase", 17463417680836924264ULL),
      goldenCase("freeze", 12816746611816117225ULL),
      goldenCase("moth", 16308746009625321247ULL),
      goldenCase("locker", 3388930426652436210ULL),
      goldenCase("witness", 12222381865618191580ULL),
      goldenCase("repair", 14430012839130274652ULL),
      goldenCase("power", 5008915058096031097ULL),
      goldenCase("grenade", 10929154195232234991ULL),
      goldenCase("lure", 2627612148598187031ULL),
      goldenCase("doors", 3889474961947922463ULL),
      goldenCase("walk", 10134188619832363132ULL),
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
