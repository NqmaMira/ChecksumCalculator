#pragma once
#include "IVisitor.h"
#include "IChecksumCalculator.h"
#include "IProgressObserver.h"
#include <vector>

class ChecksumVisitor : public IVisitor {
private:
    IChecksumCalculator& calculator;
    std::vector<IProgressObserver*> observers;
    uint64_t totalSize;
    uint64_t totalProcessed = 0;

public:
    ChecksumVisitor(IChecksumCalculator& calc, uint64_t total)
        : calculator(calc), totalSize(total) { }

    void addObserver(IProgressObserver* obs) { observers.push_back(obs); }

    void visitFile(FileNode& file) override;
    void visitDirectory(DirectoryNode& dir) override;
};