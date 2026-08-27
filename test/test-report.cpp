#include <catch2/catch_all.hpp>
#include "FileNode.h"
#include "DirectoryNode.h"
#include "PlainTextReportVisitor.h"
#include <sstream>

TEST_CASE("Report Visitors generate correct format", "[report]") {
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
        PlainTextReportVisitor reporter(ss);
        root->accept(reporter);

        std::string output = ss.str();
        REQUIRE(output.find("d41d8cd98f00b204e9800998ecf8427e */root/file1.bin") != std::string::npos);
        REQUIRE(output.find("098f6bcd4621d373cade4e832627b4f6 */root/sub/file2.bin") != std::string::npos);
    }
}