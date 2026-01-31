#pragma once
#include "IChecksumCalculator.h"

class MD5Calculator : public IChecksumCalculator {
public:
    std::string calculate(std::istream& is) override;
};