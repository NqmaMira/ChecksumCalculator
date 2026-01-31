#pragma once
#include <string>
#include <istream>

class IChecksumCalculator {
public:
    virtual ~IChecksumCalculator() = default;

    virtual std::string calculate(std::istream& is) = 0;
};