#pragma once
#include "IVisitor.h"
#include "IChecksumCalculator.h"
#include "Observable.h"
#include <set>
#include <atomic>
#include <ChecksumMemento.h>

class ChecksumVisitor : public IVisitor, public Observable {
private:
    IChecksumCalculator& calculator;
    uint64_t totalSize;
    uint64_t totalProcessed = 0;
    std::set<std::string> finishedPaths;
    std::string currentFile;
    std::atomic<bool> shouldStop = false;
    bool pausePending = false;
    bool paused = false;
public:
    ChecksumVisitor(IChecksumCalculator& calc, uint64_t total)
        : calculator(calc), totalSize(total) { }

    void visitFile(FileNode& file) override;
    void visitDirectory(DirectoryNode& dir) override;

    void restoreFromMemento(const ChecksumMemento& memento) {
        this->totalProcessed = memento.getProcessedBytes();
        this->finishedPaths = memento.getCompletedFiles();
        this->currentFile = memento.getCurrentFile();
        this->pausePending = false;
        this->paused = false;
    }

    uint64_t getTotalProcessed() const {
        return totalProcessed;
    }

    const std::set<std::string>& getCompletedFiles() const {
        return finishedPaths;
    }

    const std::string& getCurrentFile() const {
        return currentFile;
    }

    std::unique_ptr<ChecksumMemento> createMemento() const {
        return std::make_unique<ChecksumMemento>(totalProcessed, finishedPaths, currentFile);
    }

    void stop() {
        shouldStop.store(true);
    }
    bool hasStopped() const {
        return shouldStop.load();
    }
    bool hasPaused() const {
        return paused;
    }
    void resume() {
        paused = false;
        pausePending = false;
        clearPauseRequest();
    }
};