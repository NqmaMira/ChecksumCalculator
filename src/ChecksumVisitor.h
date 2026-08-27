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
    ChecksumVisitor(IChecksumCalculator& calc, uint64_t total);

    void visitFile(FileNode& file) override;
    void visitDirectory(DirectoryNode& dir) override;

    void restoreFromMemento(const ChecksumMemento& memento);

    uint64_t getTotalProcessed() const;

    const std::set<std::string>& getCompletedFiles() const;

    const std::string& getCurrentFile() const;

    std::unique_ptr<ChecksumMemento> createMemento() const;

    void stop();
    bool hasStopped() const;
    bool hasPaused() const;
    void resume();
};