#include "MD5Calculator.h"
#include <vector>
#include <md5.h>

std::string MD5Calculator::calculate(std::istream& is) {
    MD5 md5;
    char buffer[4096];
    while (is.read(buffer, sizeof(buffer))) {
        md5.add(buffer, is.gcount());
    }
    md5.add(buffer, is.gcount());
    return md5.getHash();
}