#include <array>
#include <cstdint>
#include <memory>
#include <string_view>

#include <doctest/doctest.h>

#include "support/HeadlessGameHarness.hpp"

namespace wheel_of_fear {
  namespace {
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

    constexpr int frame_count     = 1200;
    constexpr int sample_interval = 15;

    constexpr std::array<GoldenMasterCaseProps, 16> golden_master_cases {
      goldenCase("none", 16467131506377287946ULL),
      goldenCase("play", 15614293038488856873ULL),
      goldenCase("deep", 9222236461530351679ULL),
      goldenCase("tracker", 10485458730266733577ULL),
      goldenCase("sectors", 3732755655864352810ULL),
      goldenCase("showcase", 10496593956240203542ULL),
      goldenCase("freeze", 4994920308019554794ULL),
      goldenCase("moth", 3531764166164114164ULL),
      goldenCase("locker", 6229823768048464017ULL),
      goldenCase("witness", 10498217765680320769ULL),
      goldenCase("repair", 15054823844236158264ULL),
      goldenCase("power", 8452166042667028858ULL),
      goldenCase("grenade", 3082578536531543162ULL),
      goldenCase("lure", 3500444812547284908ULL),
      goldenCase("doors", 5094187250472935369ULL),
      goldenCase("walk", 7360409517096963973ULL),
    };

    std::uint64_t fingerprintOf(
      std::string_view demo_mode
    ) {
      const auto harness =
        std::make_unique<HeadlessGameHarness>(std::string(demo_mode));

      return harness->runFrames(frame_count, sample_interval);
    }
  } // namespace

  TEST_CASE(
    "headless simulation is deterministic across runs"
  ) {
    CHECK(fingerprintOf("play") == fingerprintOf("play"));
  }

  TEST_CASE(
    "every demo scenario renders the recorded golden master"
  ) {
    for(const GoldenMasterCaseProps &golden_case : golden_master_cases) {
      CAPTURE(golden_case.demo_mode);

      CHECK(
        fingerprintOf(golden_case.demo_mode) == golden_case.expected_fingerprint
      );
    }
  }
} // namespace wheel_of_fear
