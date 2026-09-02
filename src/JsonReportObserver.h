#pragma once
#include "IProgressObserver.h"
#include <iostream>
#include <string>

class JsonReportObserver : public IProgressObserver {
private:
    std::ostream& out;
    bool streamOpen = false;
    bool hasEntries = false;

    void openStream() {
        if (!streamOpen) {
            out << "[\n";
            streamOpen = true;
        }
    }

    void closeStream() {
        if (streamOpen) {
            if (hasEntries) {
                out << "\n";
            }
            out << "]\n";
            out.flush();
            streamOpen = false;
        }
    }

    static std::string escapeJson(const std::string& value) {
        std::string escaped;
        for (char character : value) {
            switch (character) {
            case '\\': escaped += "\\\\"; break;
            case '"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += character; break;
            }
        }
        return escaped;
    }

    static std::string fileNameFromPath(const std::string& filePath) {
        const auto pos = filePath.find_last_of("/\\");
        return pos == std::string::npos ? filePath : filePath.substr(pos + 1);
    }

public:
    JsonReportObserver(std::ostream& output) : out(output) {}

    ~JsonReportObserver() {
        closeStream();
    }

    void close() {
        closeStream();
    }

    void onFileStart(const std::string& path) override {
        (void)path;
        openStream();
        out.flush();
    }

    void onFileEnd(const std::string& path, const std::string& hash) override {
        openStream();

        if (hasEntries) {
            out << ",\n";
        }
        hasEntries = true;

        out << "  { \"file\": \"" << escapeJson(fileNameFromPath(path))
            << "\", \"hash\": \"" << escapeJson(hash) << "\" }";
        out.flush();
    }
};