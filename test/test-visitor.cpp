#include "catch2/catch_all.hpp"
#include "ChecksumVisitor.h"
#include "MD5Calculator.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

class MockObserver : public IProgressObserver {
public:
    int filesStarted = 0;
    int filesFinished = 0;
    uint64_t lastBytes = 0;
    uint64_t totalSizeExpected = 0;

    void onFileStart(const std::string&) override { filesStarted++; }
    void onBytesProcessed(uint64_t bytes, uint64_t totalBytes) override { 
        lastBytes = bytes; 
		totalSizeExpected = totalBytes;
    }
    void onFileEnd(const std::string&, const std::string&) override { filesFinished++; }
};

TEST_CASE("ChecksumVisitor traverses and notifies with real-time updates", "[visitor]") {
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
    auto root = std::make_shared<DirectoryNode>("root", testDir.string());
    auto f1 = std::make_shared<FileNode>("a.txt", path1, 5);
    auto f2 = std::make_shared<FileNode>("b.txt", path2, 5);
    root->addComponent(f1);
    root->addComponent(f2);

    MockObserver mock;
    ChecksumVisitor visitor(calc, root->getSize());
    visitor.addObserver(&mock);

    root->accept(visitor);

    SECTION("Visitor visits all files in the composite structure") {
        REQUIRE(mock.filesStarted == 2);
        REQUIRE(mock.filesFinished == 2);
    }

    SECTION("Visitor reports correct cumulative byte progression") {
        REQUIRE(mock.lastBytes == 10);
        REQUIRE(mock.totalSizeExpected == 10);
    }
    fs::remove_all(testDir);
}