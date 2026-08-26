#pragma once
#include "IProgressObserver.h"
#include "ChecksumMemento.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <memory>

class ConsoleProgressObserver : public IProgressObserver {
private:
    static constexpr double PercentageScale = 100.0;
    using Clock = std::chrono::steady_clock;
    Clock::time_point startTime = Clock::now();
    bool started = false;
    uint64_t processedBytes = 0;
    std::string currentFile;
public:
    void onFileStart(const std::string& path) override {
        currentFile = path;
        if(!started) {
            startTime = Clock::now();
            started = true;
		}
        std::cout << "[Processing]: " << path << std::endl;
    }

    void onBytesProcessed(uint64_t bytes, uint64_t totalBytes) override {
		processedBytes = bytes;
		auto now = Clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        double percentage = (totalBytes > 0) ? (static_cast<double>(bytes) / totalBytes) * PercentageScale : 0;

        std::string etaStr = "Calculating...";
        if (elapsed > 0 && bytes > 0) {
            uint64_t bytesPerSec = bytes / elapsed;
            if (bytesPerSec > 0) {
                uint64_t secondsRemaining = (totalBytes - bytes) / bytesPerSec;
                etaStr = std::to_string(secondsRemaining) + "s";
            }
        }

        std::cout << "\r[" << std::fixed << std::setprecision(2) << percentage << "%] "
            << "ETA: " << etaStr << " (" << bytes << "/" << totalBytes << " bytes)" << std::flush;
    }

    void onFileEnd(const std::string& path, const std::string& hash) override {
		currentFile.clear();
        std::cout << "\n[Done]: " << hash << "\n" << std::endl;
    }

    std::unique_ptr<ChecksumMemento> createMemento() const {
        return std::unique_ptr<ChecksumMemento>(
            new ChecksumMemento(processedBytes, {}, currentFile));
    }

    void restoreFromMemento(const ChecksumMemento& memento) {
        processedBytes = memento.getProcessedBytes();
        currentFile = memento.getCurrentFile();
    }
};
