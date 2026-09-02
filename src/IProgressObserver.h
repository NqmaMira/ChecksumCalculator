#pragma once
#include <string>

class IProgressObserver {
public:
    virtual ~IProgressObserver() = default;
    virtual void onFileStart(const std::string& path) = 0;
    virtual void onBytesProcessed(uint64_t, uint64_t) {}
    virtual void onFileEnd(const std::string& path, const std::string& hash) = 0;
};