#pragma once
#include "IVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <iostream>

class PlainTextReportVisitor : public IVisitor {
private:
    std::ostream& out;

public:
    PlainTextReportVisitor(std::ostream& output) : out(output) {}

    void visitFile(FileNode& file) override {
        out << file.getHash() << " *" << file.getPath() << "\n";
    }

    void visitDirectory(DirectoryNode& dir) override {
        for (auto& child : dir.getChildren()) {
            child->accept(*this);
        }
    }
};