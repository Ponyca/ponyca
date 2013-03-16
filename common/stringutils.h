#ifndef COMMON_STRINGUTILS_H
#define COMMON_STRINGUTILS_H

#include <string>

namespace Ponyca {
    namespace StringUtils {
        void trim(std::string &str);

        bool isWhitespace(char c);
    }
}


#endif // COMMON_STRINGUTILS_H
