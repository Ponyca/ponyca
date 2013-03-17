#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdint.h>

namespace Ponyca {
    struct coordinates {
        uint16_t x,y,z;
    };
    struct direction {
        uint16_t vert,horiz;
    };
    struct position {
        double x,y,z;
    };
}

#endif // TYPEDEFS_H
