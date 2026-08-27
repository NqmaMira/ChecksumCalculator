#pragma once
#include "FileSystemComponent.h"
#include <numeric>

class DirectoryNode : public FileSystemComponent {
private:
    std::vector<std::unique_ptr<FileSystemComponent>> children;

public:
    using FileSystemComponent::FileSystemComponent;

    void addComponent(std::unique_ptr<FileSystemComponent> component);
    const std::vector<std::unique_ptr<FileSystemComponent>>& getChildren() const;
    uint64_t getSize() const override;
    bool isDirectory() const override;

    void accept(IVisitor& visitor);
};