#include <catch2/catch_all.hpp>
#include "VerificationVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <iostream>
#include <VerificationReporter.h>

TEST_CASE("Verification logic handles all 4 cases", "[verification]") {
    std::map<std::string, std::string> saved = {
        {"/root/unchanged.bin", "aaaa"},
        {"/root/changed.bin", "bbbb"},
        {"/root/removed.bin", "cccc"}
    };

    auto root = std::make_shared<DirectoryNode>("root", "/root");

    auto f_unchanged = std::make_shared<FileNode>("unchanged.bin", "/root/unchanged.bin", 10);
    f_unchanged->setHash("aaaa");

    auto f_changed = std::make_shared<FileNode>("changed.bin", "/root/changed.bin", 10);
    f_changed->setHash("xxxx");

    auto f_new = std::make_shared<FileNode>("new.bin", "/root/new.bin", 10);
    f_new->setHash("dddd");

    root->addComponent(f_unchanged);
    root->addComponent(f_changed);
    root->addComponent(f_new);

    VerificationVisitor verifier(saved);
    root->accept(verifier);
    verifier.finalize();

    auto results = verifier.getResults();

    SECTION("Identify all statuses correctly") {
        int countNew = 0, countRemoved = 0, countChanged = 0, countUnchanged = 0;

        for (auto& res : results) {
            if (res.status == VerificationStatus::NEW) countNew++;
            if (res.status == VerificationStatus::REMOVED) countRemoved++;
            if (res.status == VerificationStatus::CHANGED) countChanged++;
            if (res.status == VerificationStatus::UNCHANGED) countUnchanged++;
        }

        REQUIRE(countUnchanged == 1);
        REQUIRE(countChanged == 1);
        REQUIRE(countNew == 1);
        REQUIRE(countRemoved == 1);
    }
    SECTION("Console output format check") {
        std::vector<VerificationResult> results = {
            {"file1.txt", VerificationStatus::UNCHANGED},
            {"file2.txt", VerificationStatus::CHANGED}
        };

        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        printVerificationReport(results);

        std::cout.rdbuf(old);
        std::string out = buffer.str();

        REQUIRE(out.find("[OK]       file1.txt\n") != std::string::npos);
        REQUIRE(out.find("[CHANGED]  file2.txt\n") != std::string::npos);
    }
}