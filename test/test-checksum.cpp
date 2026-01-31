#include "catch2/catch_all.hpp"
#include "Md5Calculator.h"
#include <sstream>

TEST_CASE("MD5 Calculator verifies known strings", "[checksum]") {
    MD5Calculator calc;

    SECTION("Hash of 'abc'") {
        std::stringstream ss("abc");
        REQUIRE(calc.calculate(ss) == "900150983cd24fb0d6963f7d28e17f72");
    }

    SECTION("Hash of 'Hello world!'") {
        std::stringstream ss("Hello world!");
        REQUIRE(calc.calculate(ss) == "86fb269d190d2c85f6e0468ceca42a20");
    }
}