#pragma once
#include "IVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <iostream>
#include <string>
#include <vector>

class JsonReportVisitor : public IVisitor {
private:
    std::ostream& out;
    int indent = 0;

    void printIndent() { 
        for (int i = 0; i < indent; ++i) {
            out << "  "; 
        }
    }

    static std::string escapeJson(const std::string& value) {
        std::string escaped;
        for (char character : value) {
            switch (character) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += character; break;
            }
        }
        return escaped;
    }

public:
    JsonReportVisitor(std::ostream& output) : out(output) {}

    void visitFile(FileNode& file) override {
        printIndent();
        out << "{ \"file\": \"" << escapeJson(file.getName())
            << "\", \"hash\": \"" << escapeJson(file.getHash()) << "\" }";
    }

    void visitDirectory(DirectoryNode& dir) override {
        const bool isRoot = indent == 0;
        if (isRoot) {
            out << "{\n";
            indent++;
        }

        printIndent();
        out << "\"" << escapeJson(dir.getName()) << "\": [\n";
        indent++;

        const auto& children = dir.getChildren();
        for (size_t i = 0; i < children.size(); ++i) {
            children[i]->accept(*this);
            if (i < children.size() - 1) out << ",";
            out << "\n";
        }

        indent--;
        printIndent();
        out << "]";

        if (isRoot) {
            out << "\n}\n";
            indent--;
        }
    }
};