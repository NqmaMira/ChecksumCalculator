#include "catch2/catch_all.hpp"
#include "DirectoryTreeBuilder.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void create_dummy_file(const fs::path& p, size_t size) {
    std::ofstream ofs(p, std::ios::binary);
    std::vector<char> dummy(size, 0);
    ofs.write(dummy.data(), size);
}

TEST_CASE("DirectoryTreeBuilder creates correct structure", "[builder]") {
    fs::path testDir = fs::current_path() / "test_root";
    fs::create_directories(testDir / "subdir");

    create_dummy_file(testDir / "file1.dat", 100);
    create_dummy_file(testDir / "subdir/file2.dat", 250);

    DirectoryTreeBuilder builder;
    auto root = builder.build(testDir.string());

    SECTION("Tree reflects disk hierarchy and sizes") {
        REQUIRE(root->isDirectory() == true);
        REQUIRE(root->getSize() == 350);
    }

    SECTION("Cycle detection via symlinks") {
        #ifndef _WIN32
        fs::create_directory_symlink(testDir, testDir / "subdir/loop");

        auto rootWithLoop = builder.build(testDir.string());
        REQUIRE(rootWithLoop->getSize() == 350);
        #endif
    }

    fs::remove_all(testDir);
}