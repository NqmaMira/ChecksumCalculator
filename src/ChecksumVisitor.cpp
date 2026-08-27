#include "ChecksumVisitor.h"
#include "FileNode.h"
#include "DirectoryNode.h"
#include <fstream>
#include <iostream>

ChecksumVisitor::ChecksumVisitor(IChecksumCalculator& calc, uint64_t total)
    : calculator(calc), totalSize(total) { }

void ChecksumVisitor::restoreFromMemento(const ChecksumMemento& memento) {
    this->totalProcessed = memento.getProcessedBytes();
    this->finishedPaths = memento.getCompletedFiles();
    this->currentFile = memento.getCurrentFile();
    this->pausePending = false;
    this->paused = false;
}

uint64_t ChecksumVisitor::getTotalProcessed() const {
    return totalProcessed;
}

const std::set<std::string>& ChecksumVisitor::getCompletedFiles() const {
    return finishedPaths;
}

const std::string& ChecksumVisitor::getCurrentFile() const {
    return currentFile;
}

std::unique_ptr<ChecksumMemento> ChecksumVisitor::createMemento() const {
    return std::make_unique<ChecksumMemento>(totalProcessed, finishedPaths, currentFile);
}

void ChecksumVisitor::stop() {
    shouldStop.store(true);
}

bool ChecksumVisitor::hasStopped() const {
    return shouldStop.load();
}

bool ChecksumVisitor::hasPaused() const {
    return paused;
}

void ChecksumVisitor::resume() {
    paused = false;
    pausePending = false;
    clearPauseRequest();
}

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