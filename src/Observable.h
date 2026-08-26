#pragma once
#include "IProgressObserver.h"
#include <algorithm>
#include <atomic>
#include <vector>

class Observable {
private:
    std::vector<IProgressObserver*> observers;
    std::atomic<bool> pauseRequested = false;

protected:
    void notifyFileStart(const std::string& path) {
        for (auto* observer : observers) {
            observer->onFileStart(path);
        }
    }

    void notifyBytesProcessed(uint64_t bytes, uint64_t totalBytes) {
        for (auto* observer : observers) {
            observer->onBytesProcessed(bytes, totalBytes);
        }
    }

    void notifyFileEnd(const std::string& path, const std::string& hash) {
        for (auto* observer : observers) {
            observer->onFileEnd(path, hash);
        }
    }

public:
    void requestPause() { 
        pauseRequested.store(true); 
    }
    void clearPauseRequest() { 
        pauseRequested.store(false); 
    }
    bool isPauseRequested() const { 
        return pauseRequested.load(); 
    }

    void addObserver(IProgressObserver* observer) {
        observers.push_back(observer);
    }

    void removeObserver(IProgressObserver* observer) {
        observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
    }
};