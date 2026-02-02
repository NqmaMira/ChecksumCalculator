#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>

void ChecksumVisitor::visitFile(FileNode& file) {
    for (auto* obs : observers) obs->onFileStart(file.getPath());

    std::ifstream ifs(file.getPath(), std::ios::binary);
    std::string hash = calculator.calculate(ifs);

    totalProcessed += file.getSize();
    for (auto* obs : observers) {
        obs->onBytesProcessed(totalProcessed, totalSize);
        obs->onFileEnd(file.getPath(), hash);
    }
}

void ChecksumVisitor::visitDirectory(DirectoryNode& dir) {
    for (auto& child : dir.getChildren()) {
        child->accept(*this);
    }
}