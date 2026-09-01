#include "catch2/catch_all.hpp"
#include "DirectoryTreeBuilder.h"
#include "test-filesystem.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void create_dummy_file(const fs::path& p, size_t size) {
    test_support::writeRepeated(p, size, 0);
}

TEST_CASE("DirectoryTreeBuilder creates correct structure", "[builder]") {
    constexpr size_t FirstFileSize = 100;
    constexpr size_t SecondFileSize = 250;
    constexpr size_t ExpectedDirectorySize = FirstFileSize + SecondFileSize;
    test_support::TemporaryDirectory temporaryDirectory;
    fs::path testDir = temporaryDirectory.path() / "test_root";
    fs::create_directories(testDir / "subdir");

    create_dummy_file(testDir / "file1.dat", FirstFileSize);
    create_dummy_file(testDir / "subdir/file2.dat", SecondFileSize);

    DirectoryTreeBuilder builder;
    auto root = builder.build(testDir.string());

    SECTION("Tree reflects disk hierarchy and sizes") {
        REQUIRE(root->isDirectory() == true);
        REQUIRE(root->getSize() == ExpectedDirectorySize);
    }

    SECTION("Cycle detection via symlinks") {
        #ifndef _WIN32
        fs::create_directory_symlink(testDir, testDir / "subdir/loop");

        auto rootWithLoop = builder.build(testDir.string());
        REQUIRE(rootWithLoop->getSize() == ExpectedDirectorySize);
        #endif
    }

}

TEST_CASE("DirectoryTreeBuilder handles non-existent paths", "[builder]") {
    DirectoryTreeBuilder builder;
    SECTION("Throws on non-existent path") {
        REQUIRE_THROWS_AS(builder.build("non_existent_path"), std::runtime_error);
    }
}