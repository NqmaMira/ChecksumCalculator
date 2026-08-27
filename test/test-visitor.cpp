#include "catch2/catch_all.hpp"
#include "ChecksumVisitor.h"
#include "MD5Calculator.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <filesystem>
#include <functional>

namespace fs = std::filesystem;

class MockObserver : public IProgressObserver {
public:
    int filesStarted = 0;
    int filesFinished = 0;
    uint64_t lastBytes = 0;
    uint64_t totalSizeExpected = 0;
    std::function<void()> onProgress;

    void onFileStart(const std::string&) override { filesStarted++; }
    void onBytesProcessed(uint64_t bytes, uint64_t totalBytes) override { 
        lastBytes = bytes; 
		totalSizeExpected = totalBytes;
        if (onProgress) {
            onProgress();
        }
    }
    void onFileEnd(const std::string&, const std::string&) override { filesFinished++; }
};

TEST_CASE("ChecksumVisitor traverses and notifies with real-time updates", "[visitor]") {
    constexpr size_t FileSize = 5;
    constexpr size_t ExpectedTotalSize = 2 * FileSize;
    fs::path testDir = fs::current_path() / "visitor_test_temp";
    fs::create_directory(testDir);

    auto create_file = [&](const std::string& name, const std::string& content) {
        std::ofstream ofs(testDir / name, std::ios::binary);
        ofs << content;
        return (testDir / name).string();
        };

    std::string path1 = create_file("a.txt", "12345");
    std::string path2 = create_file("b.txt", "67890");
    
    MD5Calculator calc;
    auto root = std::make_unique<DirectoryNode>("root", testDir.string());
    auto f1 = std::make_unique<FileNode>("a.txt", path1, FileSize);
    auto f2 = std::make_unique<FileNode>("b.txt", path2, FileSize);
    root->addComponent(std::move(f1));
    root->addComponent(std::move(f2));

    MockObserver mock;
    ChecksumVisitor visitor(calc, root->getSize());
    visitor.addObserver(&mock);

    root->accept(visitor);

    SECTION("Visitor visits all files in the composite structure") {
        REQUIRE(mock.filesStarted == 2);
        REQUIRE(mock.filesFinished == 2);
    }

    SECTION("Visitor reports correct cumulative byte progression") {
        REQUIRE(mock.lastBytes == ExpectedTotalSize);
        REQUIRE(mock.totalSizeExpected == ExpectedTotalSize);
    }
    fs::remove_all(testDir);
}

TEST_CASE("ChecksumVisitor with Memento and Real Files", "[visitor]") {
    constexpr size_t TestFileSize = 10000;
    fs::path tempFile = fs::current_path() / "test_data.bin";
    {
        std::ofstream ofs(tempFile, std::ios::binary);
        ofs << std::string(TestFileSize, 'A');
    }

    MD5Calculator calc;
    auto fileNode = std::make_unique<FileNode>("test_data.bin", tempFile.string(), TestFileSize);

    SECTION("Normal Visit") {
        ChecksumVisitor visitor(calc, TestFileSize);
        fileNode->accept(visitor);
        REQUIRE(visitor.getTotalProcessed() == TestFileSize);
    }

    SECTION("Memento Skip Logic") {
        ChecksumVisitor visitor1(calc, TestFileSize);
        fileNode->accept(visitor1);

        auto memento = visitor1.createMemento();

        ChecksumVisitor visitor2(calc, TestFileSize);
        visitor2.restoreFromMemento(*memento);

        fileNode->accept(visitor2);
        REQUIRE(visitor2.getTotalProcessed() == TestFileSize);
    }

    fs::remove(tempFile);
}

TEST_CASE("ChecksumVisitor pauses at a file boundary and resumes", "[pause][resume]") {
    constexpr size_t FileSize = 4 * 1024;
    constexpr size_t TotalSize = 2 * FileSize;
    fs::path testDir = fs::current_path() / "pause_resume_test_temp";
    fs::create_directory(testDir);

    auto path1 = (testDir / "a.bin").string();
    auto path2 = (testDir / "b.bin").string();
    std::ofstream(path1, std::ios::binary) << std::string(FileSize, 'a');
    std::ofstream(path2, std::ios::binary) << std::string(FileSize, 'b');

    auto root = std::make_unique<DirectoryNode>("root", testDir.string());
    auto file1 = std::make_unique<FileNode>("a.bin", path1, FileSize);
    auto file2 = std::make_unique<FileNode>("b.bin", path2, FileSize);
    auto& file1Ref = *file1;
    auto& file2Ref = *file2;
    root->addComponent(std::move(file1));
    root->addComponent(std::move(file2));

    MD5Calculator calculator;
    ChecksumVisitor visitor(calculator, TotalSize);
    visitor.requestPause();

    root->accept(visitor);

    REQUIRE(visitor.hasPaused());
    REQUIRE(visitor.getTotalProcessed() == FileSize);
    REQUIRE_FALSE(file1Ref.getHash().empty());
    REQUIRE(file2Ref.getHash().empty());

    visitor.resume();
    root->accept(visitor);

    REQUIRE_FALSE(visitor.hasPaused());
    REQUIRE(visitor.getTotalProcessed() == TotalSize);
    REQUIRE_FALSE(file2Ref.getHash().empty());

    fs::remove_all(testDir);
}

TEST_CASE("ChecksumVisitor stops traversal after a stop request", "[stop]") {
    constexpr size_t FileSize = 4 * 1024;
    constexpr size_t TotalSize = 2 * FileSize;
    fs::path testDir = fs::current_path() / "stop_test_temp";
    fs::create_directory(testDir);

    auto path1 = (testDir / "a.bin").string();
    auto path2 = (testDir / "b.bin").string();
    std::ofstream(path1, std::ios::binary) << std::string(FileSize, 'a');
    std::ofstream(path2, std::ios::binary) << std::string(FileSize, 'b');

    auto root = std::make_unique<DirectoryNode>("root", testDir.string());
    auto file1 = std::make_unique<FileNode>("a.bin", path1, FileSize);
    auto file2 = std::make_unique<FileNode>("b.bin", path2, FileSize);
    auto& file1Ref = *file1;
    auto& file2Ref = *file2;
    root->addComponent(std::move(file1));
    root->addComponent(std::move(file2));

    MD5Calculator calculator;
    ChecksumVisitor visitor(calculator, TotalSize);
    MockObserver observer;
    observer.onProgress = [&visitor]() { 
        visitor.stop(); 
    };
    visitor.addObserver(&observer);

    root->accept(visitor);

    REQUIRE(visitor.hasStopped());
    REQUIRE(visitor.getTotalProcessed() == FileSize);
    REQUIRE_FALSE(file1Ref.getHash().empty());
    REQUIRE(file2Ref.getHash().empty());

    fs::remove_all(testDir);
}