#pragma once
#include <string>
#include <istream>
#include <functional>

class IChecksumCalculator {
private:
    static constexpr std::size_t ChecksumBufferSize = 4 * 1024;

public:
    virtual ~IChecksumCalculator() = default;

    std::string calculate(std::istream& is, std::function<void(size_t)> progressCallback = nullptr,
        std::function<bool()> pauseRequested = nullptr) {
        reset();
        char buffer[ChecksumBufferSize];
        while (is.read(buffer, sizeof(buffer))) {
			size_t bytesRead = is.gcount();
            update(buffer, bytesRead);
            if(progressCallback)
				progressCallback(bytesRead);
            if (pauseRequested) {
                pauseRequested();
            }
        }
        size_t finalBytes = is.gcount();
        if (finalBytes > 0) {
            update(buffer, finalBytes);
            if (progressCallback) 
                progressCallback(finalBytes);
            if (pauseRequested) {
                pauseRequested();
            }
        }
        return getResult();
    }
protected:
    virtual void reset() = 0;
    virtual void update(const void* data, size_t length) = 0;
    virtual std::string getResult() const = 0;
};