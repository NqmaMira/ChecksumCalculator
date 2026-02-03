#pragma once
#include "IVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <map>
#include <string>
#include <vector>
#include <set>

enum class VerificationStatus {
    NEW,  
    REMOVED, 
    CHANGED, 
    UNCHANGED   
};

struct VerificationResult {
    std::string path;
    VerificationStatus status;
};

class VerificationVisitor : public IVisitor {
private:
    const std::map<std::string, std::string>& savedHashes;
    std::vector<VerificationResult> results;
    std::set<std::string> seenInTarget;

public:
    VerificationVisitor(const std::map<std::string, std::string>& saved)
        : savedHashes(saved) {
    }

    void visitFile(FileNode& file) override {
        std::string path = file.getPath();
        seenInTarget.insert(path);

        if (savedHashes.find(path) == savedHashes.end()) {
            results.push_back({ path, VerificationStatus::NEW });
        }
        else if (savedHashes.at(path) != file.getHash()) {
            results.push_back({ path, VerificationStatus::CHANGED });
        }
        else {
            results.push_back({ path, VerificationStatus::UNCHANGED });
        }
    }

    void visitDirectory(DirectoryNode& dir) override {
        for (auto& child : dir.getChildren()) {
            child->accept(*this);
        }
    }

    void finalize() {
        for (auto const& [path, hash] : savedHashes) {
            if (seenInTarget.find(path) == seenInTarget.end()) {
                results.push_back({ path, VerificationStatus::REMOVED });
            }
        }
    }

    const std::vector<VerificationResult>& getResults() const { return results; }
};