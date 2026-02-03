#include <iostream>
#include <vector>
#include "VerificationVisitor.h"

void printVerificationReport(const std::vector<VerificationResult>& results) {
    std::cout << "\n--- Verification Results ---\n";
    for (const auto& res : results) {
        std::string statusStr;
        switch (res.status) {
        case VerificationStatus::NEW:       statusStr = "[NEW]     "; break;
        case VerificationStatus::REMOVED:   statusStr = "[REMOVED] "; break;
        case VerificationStatus::CHANGED:   statusStr = "[CHANGED] "; break;
        case VerificationStatus::UNCHANGED: statusStr = "[OK]      "; break;
        }
        std::cout << statusStr << " " << res.path << "\n";
    }
}