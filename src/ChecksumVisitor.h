#pragma once
#include "IVisitor.h"
#include "IChecksumCalculator.h"
#include "IProgressObserver.h"
#include <vector>
#include <set>
#include <ChecksumMemento.h>

class ChecksumVisitor : public IVisitor {
private:
    IChecksumCalculator& calculator;
    std::vector<IProgressObserver*> observers;
    uint64_t totalSize;
    uint64_t totalProcessed = 0;
    std::set<std::string> finishedPaths;
    bool shouldStop = false;
public:
    ChecksumVisitor(IChecksumCalculator& calc, uint64_t total)
        : calculator(calc), totalSize(total) { }

    void addObserver(IProgressObserver* obs) { observers.push_back(obs); }

    void visitFile(FileNode& file) override;
    void visitDirectory(DirectoryNode& dir) override;

    void restoreFromMemento(const ChecksumMemento& memento) {
        this->totalProcessed = memento.savedProcessedBytes;
        this->finishedPaths = memento.completedFiles;
    }

    uint64_t getTotalProcessed() const { return totalProcessed; }
    std::unique_ptr<ChecksumMemento> createMemento() const {
        return std::unique_ptr<ChecksumMemento>(new ChecksumMemento(totalProcessed, finishedPaths));
    }
    void stop() { shouldStop = true; }
    bool hasStopped() const { return shouldStop; }
};