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
        : savedHashes(saved) { }

    void visitFile(FileNode& file) override;
    void visitDirectory(DirectoryNode& dir) override;
    void finalize();

    const std::vector<VerificationResult>& getResults() const { return results; }
};