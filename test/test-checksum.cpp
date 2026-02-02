#include "catch2/catch_all.hpp"
#include "Md5Calculator.h"
#include <sstream>
#include <DirectoryNode.h>
#include <FileNode.h>

TEST_CASE("MD5 Calculator verifies known strings", "[checksum]") {
    MD5Calculator calc;

    SECTION("Hash of 'abc'") {
        std::stringstream ss("abc");
        REQUIRE(calc.calculate(ss) == "900150983cd24fb0d6963f7d28e17f72");
    }

    SECTION("Hash of 'Hello world!'") {
        std::stringstream ss("Hello world!");
        REQUIRE(calc.calculate(ss) == "86fb269d190d2c85f6e0468ceca42a20");
    }
}

TEST_CASE("Composite pattern correctly calculates sizes", "[composite]") {
    auto root = std::make_shared<DirectoryNode>("root", "/root");
    auto file1 = std::make_shared<FileNode>("file1.txt", "/root/file1.txt", 1000);
    auto subDir = std::make_shared<DirectoryNode>("subdir", "/root/subdir");
    auto file2 = std::make_shared<FileNode>("file2.txt", "/root/subdir/file2.txt", 500);

    subDir->addComponent(file2);
    root->addComponent(file1);
    root->addComponent(subDir);

    SECTION("File nodes return their individual size") {
        REQUIRE(file1->getSize() == 1000);
        REQUIRE(file2->getSize() == 500);
    }

    SECTION("Directory nodes return the sum of children") {
        REQUIRE(subDir->getSize() == 500);
        REQUIRE(root->getSize() == 1500);
    }
}

TEST_CASE("Calculator callback logic", "[checksum]") {
    MD5Calculator calc;

    SECTION("Callback receives correct chunk sizes") {
        std::string data(10000, 'x');
        std::stringstream ss(data);

        std::vector<size_t> chunks;
        calc.calculate(ss, [&](size_t bytes) {
            chunks.push_back(bytes);
            });

        REQUIRE(chunks.size() == 3);
        REQUIRE(chunks[0] == 4096);
        REQUIRE(chunks[1] == 4096);
        REQUIRE(chunks[2] == 1808);

        size_t total = std::accumulate(chunks.begin(), chunks.end(), size_t(0));
        REQUIRE(total == 10000);
    }

    SECTION("Callback works with empty streams") {
        std::stringstream ss("");
        bool called = false;
        calc.calculate(ss, [&](size_t) { called = true; });

        REQUIRE_FALSE(called); 
    }
}