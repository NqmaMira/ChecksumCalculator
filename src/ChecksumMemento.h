#pragma once
#include <string>
#include <set>
#include <cstdint>

class ChecksumMemento {
private:
    uint64_t savedProcessedBytes;
    std::set<std::string> completedFiles;
    std::string currentFile;

public:
    ChecksumMemento(uint64_t bytes, std::set<std::string> files, std::string file = {})
        : savedProcessedBytes(bytes), completedFiles(std::move(files)), currentFile(std::move(file)) { }

    virtual ~ChecksumMemento() = default;
    uint64_t getProcessedBytes() const { 
        return savedProcessedBytes; 
    }
    const std::set<std::string>& getCompletedFiles() const { 
        return completedFiles; 
    }
    const std::string& getCurrentFile() const { 
        return currentFile; 
    }
};