#pragma once
class FileNode;
class DirectoryNode;

class IVisitor {
public:
    virtual ~IVisitor() = default;
    virtual void visitFile(FileNode& file) = 0;
    virtual void visitDirectory(DirectoryNode& dir) = 0;
};