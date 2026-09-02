#include <catch2/catch_all.hpp>
#include "FileNode.h"
#include "DirectoryNode.h"
#include "JsonReportObserver.h"
#include "PlainTextReportObserver.h"
#include <sstream>

TEST_CASE("Report Observers generate correct format", "[report]") {
    constexpr size_t FirstFileSize = 100;
    constexpr size_t SecondFileSize = 200;
    auto root = std::make_unique<DirectoryNode>("root", "/root");
    auto f1 = std::make_unique<FileNode>("file1.bin", "/root/file1.bin", FirstFileSize);
    f1->setHash("d41d8cd98f00b204e9800998ecf8427e");

    auto sub = std::make_unique<DirectoryNode>("sub", "/root/sub");
    auto f2 = std::make_unique<FileNode>("file2.bin", "/root/sub/file2.bin", SecondFileSize);
    f2->setHash("098f6bcd4621d373cade4e832627b4f6");

    sub->addComponent(std::move(f2));
    root->addComponent(std::move(f1));
    root->addComponent(std::move(sub));

    SECTION("Plain Text (Linux *sum format)") {
        std::stringstream ss;
        PlainTextReportObserver reporter(ss);

        reporter.onFileEnd("/root/file1.bin", "d41d8cd98f00b204e9800998ecf8427e");
        reporter.onFileEnd("/root/sub/file2.bin", "098f6bcd4621d373cade4e832627b4f6");

        const std::string output = ss.str();
        REQUIRE(output.find("d41d8cd98f00b204e9800998ecf8427e */root/file1.bin") != std::string::npos);
        REQUIRE(output.find("098f6bcd4621d373cade4e832627b4f6 */root/sub/file2.bin") != std::string::npos);
    }

    SECTION("JSON array format") {
        std::stringstream ss;
        JsonReportObserver reporter(ss);

        reporter.onFileEnd("/root/file1.bin", "d41d8cd98f00b204e9800998ecf8427e");
        reporter.onFileEnd("/root/sub/file2.bin", "098f6bcd4621d373cade4e832627b4f6");
        reporter.close();

        const std::string output = ss.str();
        REQUIRE(output.find("[\n") == 0);
        REQUIRE(output.size() >= 3);
        REQUIRE(output.find("\"file\": \"file1.bin\"") != std::string::npos);
        REQUIRE(output.find("\"hash\": \"d41d8cd98f00b204e9800998ecf8427e\"") != std::string::npos);
        REQUIRE(output.find("\"file\": \"file2.bin\"") != std::string::npos);
        REQUIRE(output.find("\"hash\": \"098f6bcd4621d373cade4e832627b4f6\"") != std::string::npos);
        REQUIRE(output.find("\n]\n") != std::string::npos);
    }

    SECTION("Report observers stream output as files finish") {
        std::stringstream ss;
        PlainTextReportObserver reporter(ss);

        reporter.onFileEnd("/root/file1.bin", "d41d8cd98f00b204e9800998ecf8427e");
        reporter.onFileEnd("/root/sub/file2.bin", "098f6bcd4621d373cade4e832627b4f6");

        const std::string output = ss.str();
        REQUIRE(output.find("d41d8cd98f00b204e9800998ecf8427e */root/file1.bin") != std::string::npos);
        REQUIRE(output.find("098f6bcd4621d373cade4e832627b4f6 */root/sub/file2.bin") != std::string::npos);
    }
}