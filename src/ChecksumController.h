#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <istream>
#include <memory>
#include <ostream>
#include <thread>

#include "ChecksumVisitor.h"
#include "ConsoleProgressObserver.h"
#include "FileSystemComponent.h"

class ChecksumController {
public:
    using CommandAvailable = std::function<bool()>;
    using ReadCommand = std::function<std::string()>;

    ChecksumController(FileSystemComponent& root, ChecksumVisitor& hasher,
        ConsoleProgressObserver& progressReporter, std::ostream& output,
        CommandAvailable commandAvailable, ReadCommand readCommand,
        std::chrono::milliseconds scanInterval = std::chrono::milliseconds(50));
    ~ChecksumController();

    ChecksumController(const ChecksumController&) = delete;
    ChecksumController& operator=(const ChecksumController&) = delete;

    bool run();

private:
    void startScan();
    void waitForScan();
    void handleCommand(const std::string& command);
    void joinScanThread();

    FileSystemComponent& root;
    ChecksumVisitor& hasher;
    ConsoleProgressObserver& progressReporter;
    std::ostream& output;
    CommandAvailable commandAvailable;
    ReadCommand readCommand;
    std::chrono::milliseconds scanInterval;
    std::thread scanThread;
    std::atomic<bool> scanFinished = false;
    std::unique_ptr<ChecksumMemento> savedCheckpoint;
};
