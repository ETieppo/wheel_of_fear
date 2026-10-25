#include "core/storage/TextFileStorage.hpp"
#include "mods/director/ProgressStore.hpp"
#include <doctest/doctest.h>
#include <filesystem>
#include <optional>

namespace wheel_of_fear {
  namespace {
    std::filesystem::path freshProgressPath() {
      const std::filesystem::path path = std::filesystem::temp_directory_path()
        / "wheel_of_fear_progress_store_tests.sav";
      std::filesystem::remove(path);
      return path;
    }

    constexpr DirectorSnapshotProps sector_three {
      .is_in_phase = true,
      .phase       = 3,
      .phase_seed  = 0xABCDEF0123456789ULL,
    };
  } // namespace

  TEST_CASE("progress store reloads the saved sector and seed") {
    const std::filesystem::path path = freshProgressPath();
    ProgressStore {path}.save(sector_three);

    const ProgressStore reloaded {path};

    REQUIRE(reloaded.saved().has_value());
    CHECK(reloaded.saved()->phase == sector_three.phase);
    CHECK(reloaded.saved()->phase_seed == sector_three.phase_seed);
    std::filesystem::remove(path);
  }

  TEST_CASE("progress store forgets a cleared run") {
    const std::filesystem::path path = freshProgressPath();
    ProgressStore               store {path};
    store.save(sector_three);
    store.clear();

    CHECK_FALSE(store.saved().has_value());
    CHECK_FALSE(ProgressStore {path}.saved().has_value());
  }

  TEST_CASE("progress store ignores a corrupted save") {
    const std::filesystem::path path = freshProgressPath();
    TextFileStorage {path}.write("phase=abc\nphase_seed=12\n");

    CHECK_FALSE(ProgressStore {path}.saved().has_value());
    std::filesystem::remove(path);
  }

  TEST_CASE("progress store without a file never persists") {
    ProgressStore store {std::nullopt};
    store.save(sector_three);

    CHECK_FALSE(store.saved().has_value());
  }
} // namespace wheel_of_fear
