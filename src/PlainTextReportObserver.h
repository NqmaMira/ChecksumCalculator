#pragma once
#include "IProgressObserver.h"
#include <iostream>

class PlainTextReportObserver : public IProgressObserver {
private:
    std::ostream& out;

public:
    PlainTextReportObserver(std::ostream& output) : out(output) {}

    void close() {
        out.flush();
    }

    void onFileStart(const std::string& path) override {
        (void)path;
    }

    void onFileEnd(const std::string& path, const std::string& hash) override {
        out << hash << " *" << path << "\n";
        out.flush();
    }
};