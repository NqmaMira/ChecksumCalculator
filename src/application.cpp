#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#ifdef _WIN32
#include <conio.h>
#endif

#include "DirectoryTreeBuilder.h"
#include "MD5Calculator.h"
#include "ChecksumVisitor.h"
#include "ConsoleProgressObserver.h"
#include "PlainTextReportVisitor.h"
#include "VerificationVisitor.h"
#include "ChecksumParser.h"
#include "VerificationReporter.h"
#include "ChecksumController.h"

void printUsage() {
    std::cout << "FMI Checksum Calculator\n"
        << "Usage:\n"
        << "  --path <dir> [--algorithm md5|sha1] [--out <file>] : Calculate hashes\n"
        << "  --checksums <file> [--path <dir>]                : Verify directory against file\n"
        << "  --help                                           : Show this help message\n"
        << "Commands during calculation: pause, resume, quit\n";
}

int main(int argc, char* argv[]) {
    std::string path = ".";
    std::string algorithm = "md5";
    std::string outFilePath = "";
    std::string verifyFilePath = "";
    bool isVerifyMode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--path" && i + 1 < argc) {
            path = argv[++i];
        }else if (arg == "--algorithm" && i + 1 < argc) {
            algorithm = argv[++i];
        }else if (arg == "--out" && i + 1 < argc) {
            outFilePath = argv[++i];
        }else if (arg == "--checksums" && i + 1 < argc) {
            verifyFilePath = argv[++i];
            isVerifyMode = true;
        }else if (arg == "--help") {
            printUsage();
            return 0;   
        }
    }

    try {
        DirectoryTreeBuilder builder;
        auto root = builder.build(path);

        std::unique_ptr<IChecksumCalculator> strategy;
        if (algorithm == "md5") {
            strategy = std::make_unique<MD5Calculator>();
        } else {
            throw std::runtime_error("Unsupported algorithm: " + algorithm);
        }

        ChecksumVisitor hasher(*strategy, root->getSize());
        ConsoleProgressObserver progressReporter;
        hasher.addObserver(&progressReporter);

        if (isVerifyMode) {
            std::cout << "Verifying path: " << path << " against " << verifyFilePath << "...\n";

            root->accept(hasher);

            auto savedHashes = ChecksumParser::load(verifyFilePath);
            VerificationVisitor verifier(savedHashes);
            root->accept(verifier);
            verifier.finalize();

            printVerificationReport(verifier.getResults());
        }
        else {
            std::cout << "Calculating checksums for: " << path << "...\n";
            auto commandAvailable = []() {
#ifdef _WIN32
                return _kbhit() != 0;
#else
                return true;
#endif
            };
            auto readCommand = []() {
                std::string command;
                std::getline(std::cin, command);
                return command;
            };

            ChecksumController controller(*root, hasher, progressReporter, std::cout,
                commandAvailable, readCommand);
            if (!controller.run()) {
                return 0;
            }

            if (!outFilePath.empty()) {
                std::ofstream outFile(outFilePath);
                PlainTextReportVisitor reporter(outFile);
                root->accept(reporter);
                std::cout << "\nResults exported to " << outFilePath << "\n";
            }
        }

    }
    catch (const std::exception& e) {
        std::cerr << "\n[Error]: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}