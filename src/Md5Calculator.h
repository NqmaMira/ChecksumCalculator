#pragma once
#include "IChecksumCalculator.h"
#include "../lib/hash-library-master/md5.h"

class MD5Calculator : public IChecksumCalculator {
private:
    mutable MD5 hasher;
protected:
    void reset() override {
        hasher.reset();
    }
    void update(const void* data, size_t length) override {
        hasher.add(data, length);
    }
    std::string getResult() const override {
        return hasher.getHash();
	}
};