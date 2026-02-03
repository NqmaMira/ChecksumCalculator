#pragma once
#include "FileSystemComponent.h"

class FileNode : public FileSystemComponent {
private:
    uint64_t size;
    std::string hash;

public:
    FileNode(std::string name, std::string path, uint64_t size);

    uint64_t getSize() const override;
    bool isDirectory() const override;
    void setHash(const std::string& hashValue);
    std::string getHash() const;

	void accept(IVisitor& visitor) override;
};