#include <catch2/catch_all.hpp>

#include "ChecksumController.h"
#include "DirectoryNode.h"
#include "FileNode.h"
#include "MD5Calculator.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

TEST_CASE("ChecksumController runs a checksum scan", "[controller]") {
    const fs::path testFile = fs::current_path() / "controller_test.bin";
    constexpr size_t FileSize = 4096;
    std::ofstream(testFile, std::ios::binary) << std::string(FileSize, 'x');

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

    fs::remove(testFile);
}

TEST_CASE("ChecksumController resumes a paused scan", "[controller][pause][resume]") {
    const fs::path testFile = fs::current_path() / "controller_pause_test.bin";
    constexpr size_t FileSize = 4096;
    std::ofstream(testFile, std::ios::binary) << std::string(FileSize, 'x');

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

    fs::remove(testFile);
}
