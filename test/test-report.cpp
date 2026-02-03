#include <catch2/catch_all.hpp>
#include "FileNode.h"
#include "DirectoryNode.h"
#include "PlainTextReportVisitor.h"
#include <sstream>

TEST_CASE("Report Visitors generate correct format", "[report]") {
    auto root = std::make_shared<DirectoryNode>("root", "/root");
    auto f1 = std::make_shared<FileNode>("file1.bin", "/root/file1.bin", 100);
    f1->setHash("d41d8cd98f00b204e9800998ecf8427e");

    auto sub = std::make_shared<DirectoryNode>("sub", "/root/sub");
    auto f2 = std::make_shared<FileNode>("file2.bin", "/root/sub/file2.bin", 200);
    f2->setHash("098f6bcd4621d373cade4e832627b4f6");

    sub->addComponent(f2);
    root->addComponent(f1);
    root->addComponent(sub);

    SECTION("Plain Text (Linux *sum format)") {
        std::stringstream ss;
        PlainTextReportVisitor reporter(ss);
        root->accept(reporter);

        std::string output = ss.str();
        REQUIRE(output.find("d41d8cd98f00b204e9800998ecf8427e */root/file1.bin") != std::string::npos);
        REQUIRE(output.find("098f6bcd4621d373cade4e832627b4f6 */root/sub/file2.bin") != std::string::npos);
    }
}