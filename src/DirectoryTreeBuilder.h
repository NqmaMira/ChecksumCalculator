#pragma once
#include "FileSystemComponent.h"
#include "DirectoryNode.h"
#include <filesystem>
#include <set>
#include <memory>

namespace fs = std::filesystem;

class DirectoryTreeBuilder {
public:
    std::shared_ptr<FileSystemComponent> build(const std::string& path);

private:
    std::set<fs::path> visitedPaths;
    std::shared_ptr<FileSystemComponent> processPath(const fs::path& p);
};