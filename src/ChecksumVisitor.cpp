#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <iostream>

void ChecksumVisitor::visitFile(FileNode& file) {
    const std::string path = file.getPath();

    if (shouldStop || paused || finishedPaths.count(path)) {
        return;
    }

    currentFile = path;
    notifyFileStart(path);

    std::ifstream ifs(path, std::ios::binary);

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
        finishedPaths.insert(path);
        currentFile.clear();
        notifyFileEnd(path, hash);
        if (pausePending) {
            paused = true;
        }
    }
    catch (const std::exception& e) {
		std::cerr << "Error processing file " << path << ": " << e.what() << std::endl;
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