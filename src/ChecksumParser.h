#pragma once
#include <map>
#include <string>
#include <fstream>
#include <sstream>

class ChecksumParser {
public:
    static std::map<std::string, std::string> load(const std::string& filename) {
        std::map<std::string, std::string> hashes;
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            size_t starPos = line.find(" *");
            if (starPos != std::string::npos) {
                std::string hash = line.substr(0, starPos);
                hash.erase(hash.find_last_not_of(" ") + 1);

                std::string path = line.substr(starPos + 2);
                hashes[path] = hash;
            }
        }
        return hashes;
    }
};