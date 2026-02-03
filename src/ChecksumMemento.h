#pragma once
#include <string>
#include <set>
#include <cstdint>

class ChecksumMemento {
private:
    friend class ChecksumVisitor;

    uint64_t savedProcessedBytes;
    std::set<std::string> completedFiles;

    ChecksumMemento(uint64_t bytes, std::set<std::string> files)
        : savedProcessedBytes(bytes), completedFiles(std::move(files)) { }

public:
    virtual ~ChecksumMemento() = default;
};