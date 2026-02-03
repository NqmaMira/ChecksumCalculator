#pragma once
#include "IVisitor.h"
#include <iostream>
#include <vector>

class JsonReportVisitor : public IVisitor {
private:
    std::ostream& out;
    int indent = 0;

    void printIndent() { for (int i = 0; i < indent; ++i) out << "  "; }

public:
    JsonReportVisitor(std::ostream& output) : out(output) {}

    void visitFile(FileNode& file) override {
        printIndent();
        out << "{ \"file\": \"" << file.getName()
            << "\", \"hash\": \"" << file.getHash() << "\" }";
    }

    void visitDirectory(DirectoryNode& dir) override {
        printIndent();
        out << "\"" << dir.getName() << "\": [\n";
        indent++;

        auto children = dir.getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            children[i]->accept(*this);
            if (i < children.size() - 1) out << ",";
            out << "\n";
        }

        indent--;
        printIndent();
        out << "]";
    }
};