#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

namespace test_support {
namespace fs = std::filesystem;

class TemporaryDirectory {
private:
    fs::path directory;

public:
    TemporaryDirectory() {
        static std::atomic<unsigned long long> counter{ 0 };
        const auto timestamp = std::chrono::steady_clock::now().time_since_epoch().count();
        directory = fs::temp_directory_path()
            / ("ChecksumCalculator-" + std::to_string(timestamp)
                + "-" + std::to_string(counter.fetch_add(1)));

        std::error_code error;
        if (!fs::create_directories(directory, error) || error) {
            throw std::runtime_error("Could not create temporary test directory: " + directory.string());
        }
    }

    ~TemporaryDirectory() {
        std::error_code error;
        fs::remove_all(directory, error);
    }

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

    const fs::path& path() const {
        return directory;
    }
};

inline void writeFile(const fs::path& path, const std::string& data) {
    std::ofstream output(path, std::ios::binary);
    if (!output) {
        throw std::runtime_error("Could not open test file for writing: " + path.string());
    }

    output.write(data.data(), static_cast<std::streamsize>(data.size()));
    if (!output) {
        throw std::runtime_error("Could not write test file: " + path.string());
    }
}

inline void writeRepeated(const fs::path& path, size_t size, char value) {
    writeFile(path, std::string(size, value));
}
}
