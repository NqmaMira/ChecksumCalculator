#include <catch2/catch_all.hpp>

#include "ChecksumController.h"
#include "DirectoryNode.h"
#include "FileNode.h"
#include "MD5Calculator.h"
#include "test-filesystem.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

TEST_CASE("ChecksumController runs a checksum scan", "[controller]") {
    test_support::TemporaryDirectory temporaryDirectory;
    const fs::path testFile = temporaryDirectory.path() / "controller_test.bin";
    constexpr size_t FileSize = 4096;
    test_support::writeRepeated(testFile, FileSize, 'x');

    auto root = std::make_unique<FileNode>(testFile.filename().string(), testFile.string(), FileSize);
    MD5Calculator calculator;
    ChecksumVisitor hasher(calculator, FileSize);
    ConsoleProgressObserver progressReporter;
    std::ostringstream output;

    ChecksumController controller(*root, hasher, progressReporter, output,
        [] { return false; }, [] { return std::string(); });

    REQUIRE(controller.run());
    REQUIRE_FALSE(root->getHash().empty());
    REQUIRE(hasher.getTotalProcessed() == FileSize);

}

TEST_CASE("ChecksumController resumes a paused scan", "[controller][pause][resume]") {
    test_support::TemporaryDirectory temporaryDirectory;
    const fs::path testFile = temporaryDirectory.path() / "controller_pause_test.bin";
    constexpr size_t FileSize = 4096;
    test_support::writeRepeated(testFile, FileSize, 'x');

    auto root = std::make_unique<FileNode>(testFile.filename().string(), testFile.string(), FileSize);
    MD5Calculator calculator;
    ChecksumVisitor hasher(calculator, FileSize);
    hasher.requestPause();
    ConsoleProgressObserver progressReporter;
    std::ostringstream output;
    std::vector<std::string> commands = { "resume" };
    size_t nextCommand = 0;

    ChecksumController controller(*root, hasher, progressReporter, output,
        [&] { return nextCommand < commands.size(); },
        [&] { return commands[nextCommand++]; });

    REQUIRE(controller.run());
    REQUIRE_FALSE(hasher.hasPaused());
    REQUIRE(hasher.getTotalProcessed() == FileSize);
    REQUIRE_FALSE(root->getHash().empty());

}

class ThrowingComponent : public FileSystemComponent {
public:
    ThrowingComponent() : FileSystemComponent("throwing", "throwing") {}

    uint64_t getSize() const override { 
        return 0; 
    }
    bool isDirectory() const override { 
        return false; 
    }

    void accept(IVisitor&) override {
        throw std::runtime_error("Simulated traversal failure");
    }
};

TEST_CASE("ChecksumController rethrows scan-thread errors", "[controller][exceptions]") {
    ThrowingComponent root;
    MD5Calculator calculator;
    ChecksumVisitor hasher(calculator, 0);
    ConsoleProgressObserver progressReporter;
    std::ostringstream output;

    ChecksumController controller(root, hasher, progressReporter, output,
        [] { return false; }, [] { return std::string(); });

    REQUIRE_THROWS_WITH(controller.run(), "Simulated traversal failure");
}
