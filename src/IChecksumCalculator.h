#pragma once
#include <string>
#include <istream>

class IChecksumCalculator {
public:
    virtual ~IChecksumCalculator() = default;

    std::string calculate(std::istream& is) {
        reset();
        char buffer[4096];
        while (is.read(buffer, sizeof(buffer))) {
            update(buffer, is.gcount());
        }
        update(buffer, is.gcount());
        return getResult();
    }
protected:
    virtual void reset() = 0;
    virtual void update(const void* data, size_t length) = 0;
    virtual std::string getResult() const = 0;
};