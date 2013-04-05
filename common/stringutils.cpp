#include "common/pch.h"
#include "common/stringutils.h"

void Ponyca::StringUtils::trim(std::string &str) {
    while(str.length() > 0 && isWhitespace(str.at(0))) {
        str.erase(str.begin(), str.begin()+1);
    }
    while(str.length() > 0 && isWhitespace(str.at(str.length()-1))) {
        str.erase(str.end()-1, str.end());
    }
}

bool Ponyca::StringUtils::isWhitespace(char c) {
    return c == ' ' || c == '\t';
}
