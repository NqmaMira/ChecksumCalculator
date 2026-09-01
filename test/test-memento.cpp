#include <catch2/catch_all.hpp>
#include "ChecksumVisitor.h"
#include "MD5Calculator.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include "test-filesystem.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

void write_file(const std::string& path, size_t size) {
    test_support::writeRepeated(path, size, 'x');
}

TEST_CASE("Memento saves and restores state correctly", "[memento]") {
    constexpr size_t FirstFileSize = 100;
    constexpr size_t SecondFileSize = 200;
    constexpr size_t ExpectedTotalSize = FirstFileSize + SecondFileSize;
    test_support::TemporaryDirectory temporaryDirectory;
    fs::path testDir = temporaryDirectory.path() / "memento_test";
    fs::create_directories(testDir);

    std::string p1 = (testDir / "file1.bin").string();
    std::string p2 = (testDir / "file2.bin").string();
    write_file(p1, FirstFileSize);
    write_file(p2, SecondFileSize);

    MD5Calculator calc;
    auto root = std::make_unique<DirectoryNode>("root", testDir.string());
    auto f1 = std::make_unique<FileNode>("file1.bin", p1, FirstFileSize);
    auto f2 = std::make_unique<FileNode>("file2.bin", p2, SecondFileSize);
    auto& f1Ref = *f1;
    root->addComponent(std::move(f1));
    root->addComponent(std::move(f2));

    uint64_t totalSize = root->getSize();

    SECTION("Partial execution and resume") {
        ChecksumVisitor visitor1(calc, totalSize);
        f1Ref.accept(visitor1);

        auto memento = visitor1.createMemento();
        REQUIRE(visitor1.getTotalProcessed() == FirstFileSize);

        ChecksumVisitor visitor2(calc, totalSize);
        visitor2.restoreFromMemento(*memento);

        root->accept(visitor2);

        REQUIRE(visitor2.getTotalProcessed() == ExpectedTotalSize);
    }

    SECTION("Checkpoint includes the full paused state needed for resume") {
        ChecksumVisitor visitor(calc, totalSize);
        visitor.requestPause();

        f1Ref.accept(visitor);

        REQUIRE(visitor.hasPaused());

        const auto memento = visitor.createMemento();
        REQUIRE(memento->getProcessedBytes() == visitor.getTotalProcessed());
        REQUIRE(memento->getCompletedFiles() == visitor.getCompletedFiles());
        REQUIRE(memento->getCurrentFile() == visitor.getCurrentFile());
    }

}