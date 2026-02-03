#include <catch2/catch_all.hpp>
#include "ChecksumVisitor.h"
#include "MD5Calculator.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void write_file(const std::string& path, size_t size) {
    std::ofstream ofs(path, std::ios::binary);
    std::string data(size, 'x');
    ofs << data;
}

TEST_CASE("Memento saves and restores state correctly", "[memento]") {
    fs::path testDir = fs::current_path() / "memento_test";
    fs::create_directories(testDir);

    std::string p1 = (testDir / "file1.bin").string();
    std::string p2 = (testDir / "file2.bin").string();
    write_file(p1, 100);
    write_file(p2, 200);

    MD5Calculator calc;
    auto root = std::make_shared<DirectoryNode>("root", testDir.string());
    auto f1 = std::make_shared<FileNode>("file1.bin", p1, 100);
    auto f2 = std::make_shared<FileNode>("file2.bin", p2, 200);
    root->addComponent(f1);
    root->addComponent(f2);

    uint64_t totalSize = root->getSize();

    SECTION("Partial execution and resume") {
        ChecksumVisitor visitor1(calc, totalSize);
        f1->accept(visitor1);

        auto memento = visitor1.createMemento();
        REQUIRE(visitor1.getTotalProcessed() == 100);

        ChecksumVisitor visitor2(calc, totalSize);
        visitor2.restoreFromMemento(*memento);

        root->accept(visitor2);

        REQUIRE(visitor2.getTotalProcessed() == 300);
    }

    fs::remove_all(testDir);
}