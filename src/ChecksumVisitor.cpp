#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>

void ChecksumVisitor::visitFile(FileNode& file) {
    if (finishedPaths.count(file.getPath())) {
        return;
    }

    for (auto* obs : observers) 
        obs->onFileStart(file.getPath());

    std::ifstream ifs(file.getPath(), std::ios::binary);

    auto progressLambda = [this](size_t bytesInChunk) {
        this->totalProcessed += bytesInChunk;
        for (auto* obs : this->observers) {
            obs->onBytesProcessed(this->totalProcessed, this->totalSize);
        }
    };

    std::string hash = calculator.calculate(ifs, progressLambda);
	file.setHash(hash);

	finishedPaths.insert(file.getPath());
    for (auto* obs : observers) {
        obs->onFileEnd(file.getPath(), hash);
    }
}

void ChecksumVisitor::visitDirectory(DirectoryNode& dir) {
    for (auto& child : dir.getChildren()) {
        child->accept(*this);
    }
}