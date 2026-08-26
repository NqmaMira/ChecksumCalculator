#pragma once
#include "IVisitor.h"
#include "IChecksumCalculator.h"
#include "Observable.h"
#include <set>
#include <ChecksumMemento.h>

class ChecksumVisitor : public IVisitor, public Observable {
private:
    IChecksumCalculator& calculator;
    uint64_t totalSize;
    uint64_t totalProcessed = 0;
    std::set<std::string> finishedPaths;
    bool shouldStop = false;
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
        this->pausePending = false;
        this->paused = false;
    }

    uint64_t getTotalProcessed() const { 
        return totalProcessed; 
    }

    std::unique_ptr<ChecksumMemento> createMemento() const {
        return std::unique_ptr<ChecksumMemento>(new ChecksumMemento(totalProcessed, finishedPaths));
    }

    void stop() { 
        shouldStop = true; 
    }
    bool hasStopped() const { 
        return shouldStop; 
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