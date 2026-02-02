#pragma once
#include <string>
#include <istream>
#include <functional>

class IChecksumCalculator {
public:
    virtual ~IChecksumCalculator() = default;

    std::string calculate(std::istream& is, std::function<void(size_t)> progressCallback = nullptr) {
        reset();
        char buffer[4096];
        while (is.read(buffer, sizeof(buffer))) {
			size_t bytesRead = is.gcount();
            update(buffer, bytesRead);
            if(progressCallback)
				progressCallback(bytesRead);
        }
        size_t finalBytes = is.gcount();
        if (finalBytes > 0) {
            update(buffer, finalBytes);
            if (progressCallback) 
                progressCallback(finalBytes);
        }
        return getResult();
    }
protected:
    virtual void reset() = 0;
    virtual void update(const void* data, size_t length) = 0;
    virtual std::string getResult() const = 0;
};