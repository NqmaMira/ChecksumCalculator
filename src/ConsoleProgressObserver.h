#pragma once
#include "IProgressObserver.h"
#include <iostream>
#include <iomanip>

class ConsoleProgressObserver : public IProgressObserver {
public:
    void onFileStart(const std::string& path) override {
        std::cout << "[Processing]: " << path << std::endl;
    }

    void onBytesProcessed(uint64_t bytes, uint64_t totalBytes) override {
        double percentage = (totalBytes > 0) ? (static_cast<double>(bytes) / totalBytes) * 100.0 : 0;

        std::cout << "\r[" << std::fixed << std::setprecision(2) << percentage << "%] "
            << bytes << "/" << totalBytes << " bytes" << std::flush;

        if (bytes == totalBytes) 
            std::cout << std::endl;
    }

    void onFileEnd(const std::string& path, const std::string& hash) override {
        std::cout << "[Done]: " << hash << "\n" << std::endl;
    }
};