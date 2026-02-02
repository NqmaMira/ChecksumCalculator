#include "DirectoryTreeBuilder.h"
#include "FileNode.h"
#include <iostream>

std::shared_ptr<FileSystemComponent> DirectoryTreeBuilder::build(const std::string& path) {
    visitedPaths.clear();
    fs::path rootPath(path);

    if (!fs::exists(rootPath)) {
        throw std::runtime_error("Path does not exist: " + path);
    }
    return processPath(rootPath);
}

std::shared_ptr<FileSystemComponent> DirectoryTreeBuilder::processPath(const fs::path& p) {
    fs::path canonicalP = fs::weakly_canonical(p);

    if (fs::is_directory(p)) {
        if (visitedPaths.find(canonicalP) != visitedPaths.end()) {
            return nullptr;
        }
        visitedPaths.insert(canonicalP);

        auto dirNode = std::make_shared<DirectoryNode>(p.filename().string(), p.string());
        for (const auto& entry : fs::directory_iterator(p)) {
            auto child = processPath(entry.path());
            if (child) {
                dirNode->addComponent(child);
            }
        }
        return dirNode;
    }
    else if (fs::is_regular_file(p)) {
        return std::make_shared<FileNode>(p.filename().string(), p.string(), fs::file_size(p));
    }

    return nullptr;
}