#include "catch2/catch_all.hpp"
#include "ChecksumVisitor.h"
#include "MD5Calculator.h"
#include "FileNode.h"
#include "DirectoryNode.h"

class MockObserver : public IProgressObserver {
public:
    int filesStarted = 0;
    int filesFinished = 0;
    uint64_t lastBytes = 0;

    void onFileStart(const std::string&) override { filesStarted++; }
    void onBytesProcessed(uint64_t bytes, uint64_t) override { lastBytes = bytes; }
    void onFileEnd(const std::string&, const std::string&) override { filesFinished++; }
};

TEST_CASE("ChecksumVisitor traverses and notifies", "[visitor]") {
    MD5Calculator calc;
    auto root = std::make_shared<DirectoryNode>("root", "root");
    auto f1 = std::make_shared<FileNode>("a.txt", "a.txt", 10);
    root->addComponent(f1);

    MockObserver mock;
    ChecksumVisitor visitor(calc, root->getSize());
    visitor.addObserver(&mock);

    root->accept(visitor);

    SECTION("Visitor visits all files") {
        REQUIRE(mock.filesStarted == 1);
        REQUIRE(mock.filesFinished == 1);
    }

    SECTION("Visitor reports correct byte progression") {
        REQUIRE(mock.lastBytes == 10);
    }
}