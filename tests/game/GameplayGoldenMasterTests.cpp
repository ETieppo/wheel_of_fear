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
      goldenCase("none", 13297964403152090846ULL),
      goldenCase("play", 7414532643556043029ULL),
      goldenCase("deep", 16821626604857782160ULL),
      goldenCase("tracker", 6137307892709005509ULL),
      goldenCase("sectors", 17790140775036056988ULL),
      goldenCase("showcase", 9541908370609647766ULL),
      goldenCase("freeze", 5599517969219557664ULL),
      goldenCase("moth", 5579068260883311168ULL),
      goldenCase("locker", 14524398027808874153ULL),
      goldenCase("witness", 3947612324877227682ULL),
      goldenCase("repair", 7959385061141975901ULL),
      goldenCase("power", 3108626882902100367ULL),
      goldenCase("grenade", 15574360401452003682ULL),
      goldenCase("lure", 9836432820746362474ULL),
      goldenCase("doors", 8170728131941234573ULL),
      goldenCase("walk", 258648482903730868ULL),
      goldenCase("ending", 7938435124252241230ULL),
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
