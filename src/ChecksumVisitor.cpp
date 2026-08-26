#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <iostream>

void ChecksumVisitor::visitFile(FileNode& file) {
    if (shouldStop || paused || finishedPaths.count(file.getPath())) {
        return;
    }

    notifyFileStart(file.getPath());

    std::ifstream ifs(file.getPath(), std::ios::binary);

    auto progressLambda = [this](size_t bytesInChunk) {
        this->totalProcessed += bytesInChunk;
        notifyBytesProcessed(this->totalProcessed, this->totalSize);
    };

    auto pauseLambda = [this]() {
        this->pausePending = this->isPauseRequested();
        return this->pausePending;
    };

    try {
        std::string hash = calculator.calculate(ifs, progressLambda, pauseLambda);
        file.setHash(hash);
        finishedPaths.insert(file.getPath());
        notifyFileEnd(file.getPath(), hash);
        if (pausePending) {
            paused = true;
        }
    }
    catch (const std::exception& e) {
		std::cerr << "Error processing file " << file.getPath() << ": " << e.what() << std::endl;
        return;
	}
}

void ChecksumVisitor::visitDirectory(DirectoryNode& dir) {
    if (shouldStop || paused) {
        return;
    }
    for (auto& child : dir.getChildren()) {
        child->accept(*this);
        if (paused || shouldStop) {
            return;
        }
    }
}