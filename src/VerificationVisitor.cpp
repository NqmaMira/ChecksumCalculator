#include "VerificationVisitor.h"

void VerificationVisitor::visitFile(FileNode& file) {
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

void VerificationVisitor::visitDirectory(DirectoryNode& dir) {
    for (auto& child : dir.getChildren()) {
        child->accept(*this);
    }
}

void VerificationVisitor::finalize() {
    for (auto const& [path, hash] : savedHashes) {
        if (seenInTarget.find(path) == seenInTarget.end()) {
            results.push_back({ path, VerificationStatus::REMOVED });
        }
    }
}