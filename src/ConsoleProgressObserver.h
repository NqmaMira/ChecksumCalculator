#pragma once
#include "IProgressObserver.h"
#include <iostream>
#include <iomanip>
#include <chrono>

class ConsoleProgressObserver : public IProgressObserver {
private:
    using Clock = std::chrono::steady_clock;
    Clock::time_point startTime = Clock::now();
	//const std::chrono::milliseconds updateInterval{ 100 };
    bool started = false;
public:
    void onFileStart(const std::string& path) override {
        if(!started) {
            startTime = Clock::now();
            started = true;
		}
        std::cout << "[Processing]: " << path << std::endl;
    }

    void onBytesProcessed(uint64_t bytes, uint64_t totalBytes) override {
		auto now = Clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        double percentage = (totalBytes > 0) ? (static_cast<double>(bytes) / totalBytes) * 100.0 : 0;

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
        std::cout << "\n[Done]: " << hash << "\n" << std::endl;
    }
};