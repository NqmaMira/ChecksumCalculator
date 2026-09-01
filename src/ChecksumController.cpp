#include "ChecksumController.h"

#include <thread>

ChecksumController::ChecksumController(FileSystemComponent& root, ChecksumVisitor& hasher,
    ConsoleProgressObserver& progressReporter, std::ostream& output,
    CommandAvailable commandAvailable, ReadCommand readCommand,
    std::chrono::milliseconds scanInterval)
    : root(root), hasher(hasher), progressReporter(progressReporter), output(output),
      commandAvailable(std::move(commandAvailable)), readCommand(std::move(readCommand)),
      scanInterval(scanInterval) { }

ChecksumController::~ChecksumController() {
    joinScanThread();
}

bool ChecksumController::run() {
    output << "Commands: pause, resume, quit" << std::endl;
    startScan();
    waitForScan();

    while (hasher.hasPaused()) {
        if (!savedCheckpoint) {
            savedCheckpoint = hasher.createMemento();
        }

        output << "Paused. Enter resume or quit." << std::endl;
        const std::string command = readCommand();
        if (command == "quit") {
            return false;
        }
        if (command != "resume") {
            continue;
        }

        hasher.restoreFromMemento(*savedCheckpoint);
        progressReporter.restoreFromMemento(*savedCheckpoint);
        hasher.resume();
        savedCheckpoint.reset();
        startScan();
        waitForScan();
    }

    return !hasher.hasStopped();
}

void ChecksumController::startScan() {
    scanFinished = false;
    scanException = nullptr;
    scanThread = std::thread([this]() {
        try {
            root.accept(hasher);
        }
        catch (...) {
            scanException = std::current_exception();
        }
        scanFinished = true;
    });
}

void ChecksumController::waitForScan() {
    while (!scanFinished) {
        if (commandAvailable && commandAvailable()) {
            handleCommand(readCommand());
        }
        std::this_thread::sleep_for(scanInterval);
    }
    joinScanThread();

    if (scanException) {
        std::rethrow_exception(scanException);
    }
}

void ChecksumController::handleCommand(const std::string& command) {
    if (command == "pause") {
        hasher.requestPause();
    }
    else if (command == "quit") {
        hasher.stop();
    }
    else if (command == "resume") {
        hasher.resume();
    }
}

void ChecksumController::joinScanThread() {
    if (scanThread.joinable()) {
        scanThread.join();
    }
}
