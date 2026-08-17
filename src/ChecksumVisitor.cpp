#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <iostream>

void ChecksumVisitor::visitFile(FileNode& file) {
    if (shouldStop || finishedPaths.count(file.getPath())) {
        return;
    }

    for (auto* obs : observers) 
        obs->onFileStart(file.getPath());

    std::ifstream ifs(file.getPath(), std::ios::binary);

    auto progressLambda = [this](size_t bytesInChunk) {
        if(this->shouldStop) {
            throw std::runtime_error("Processing stopped by user.");
		}
        this->totalProcessed += bytesInChunk;
        for (auto* obs : this->observers) {
            obs->onBytesProcessed(this->totalProcessed, this->totalSize);
        }
    };

    try {
        std::string hash = calculator.calculate(ifs, progressLambda);
        file.setHash(hash);
        finishedPaths.insert(file.getPath());
        for (auto* obs : observers) {
            obs->onFileEnd(file.getPath(), hash);
        }
    }
    catch (const std::exception& e) {
		std::cerr << "Error processing file " << file.getPath() << ": " << e.what() << std::endl;
        return;
	}
}

void ChecksumVisitor::visitDirectory(DirectoryNode& dir) {
    for (auto& child : dir.getChildren()) {
        child->accept(*this);
    }
}