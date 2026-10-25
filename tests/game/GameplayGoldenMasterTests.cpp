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

    constexpr std::array<GoldenMasterCaseProps, 17> golden_master_cases {
      goldenCase("none", 9671897175494544716ULL),
      goldenCase("play", 16501620738589448063ULL),
      goldenCase("deep", 2760354486160752501ULL),
      goldenCase("tracker", 10363383495987562495ULL),
      goldenCase("sectors", 70736726000219281ULL),
      goldenCase("showcase", 10875246762754108044ULL),
      goldenCase("freeze", 8938265978280193661ULL),
      goldenCase("moth", 1232797427756347883ULL),
      goldenCase("locker", 7514666287334400793ULL),
      goldenCase("witness", 18142075704131835559ULL),
      goldenCase("repair", 8770032242735356949ULL),
      goldenCase("power", 5736469575518492680ULL),
      goldenCase("grenade", 11496448755694345352ULL),
      goldenCase("lure", 13511371594272437495ULL),
      goldenCase("doors", 11539910087029074923ULL),
      goldenCase("walk", 14316038490100979017ULL),
      goldenCase("ending", 14560409433106449169ULL),
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
