#include <cstring>

#include "network.h"

using namespace Ponyca::Net;

std::string TCPPacketHeader::serialize() const {
    std::string buffer;
    buffer += opcode.serialize();
    buffer += length.serialize();
    return buffer;
}

uint16_t TCPPacketHeader::unserialize(const char *buffer, uint16_t availableBytes) {
    uint16_t offset = 0;
    offset += opcode.unserialize(buffer+offset, availableBytes-offset);
    offset += length.unserialize(buffer+offset, availableBytes-offset);
    return offset;
}


std::string DynMap::serialize() const {
    std::string buffer;
    buffer += Uint16Wrapper(map.size()).serialize();
    auto it = map.begin();
    for(; it!=map.end(); it++) {
        buffer += String(it->first).serialize();
        buffer += it->second->serialize();
    }
    return buffer;
}
uint16_t DynMap::unserialize(char const *buffer, uint16_t availableBytes) {
    uint16_t i, offset = 0;
    Uint16Wrapper length;
    offset += length.unserialize(buffer+offset, availableBytes-offset);
    for(i=0; i<length.value; i++) {
        String key;
        Int8Wrapper type;
        offset += key.unserialize(buffer+offset, availableBytes-offset);
        offset += type.unserialize(buffer+offset, availableBytes-offset);

        // FIXME: DELETE IT SOMEWHERE
        AbstractSerializable* obj = makeSerializable(type.value);
        offset += obj->unserialize(buffer+offset, availableBytes-offset);
        map[key] = obj;
    }
    return 2 + offset;
}



AbstractSerializable* Ponyca::Net::makeSerializable(int16_t typeInt) {
    Types type = (Types)typeInt;
    switch(type) {
    case Types::BOOL: return new BoolWrapper;
    case Types::INT8: return new Int8Wrapper;
    case Types::INT16: return new Int16Wrapper;
    case Types::INT32: return new Int32Wrapper;
    case Types::INT64: return new Int64Wrapper;
    case Types::UINT8: return new Uint8Wrapper;
    case Types::UINT16: return new Uint16Wrapper;
    case Types::UINT32: return new Uint32Wrapper;
    case Types::UINT64: return new Uint64Wrapper;
    case Types::FLOAT32: return new Float32Wrapper;
    case Types::FLOAT64: return new Float64Wrapper;
    case Types::STRING: return new String;
    case Types::DYNMAP: return new DynMap;
    default:; // empty, to remove -Wswitch warning
    }
    if (typeInt & (int16_t)Types::LIST_OF) {
        type = (Types)(typeInt & ~(int16_t)Types::LIST_OF);
        switch(type) {
        case Types::BOOL: return new List<BoolWrapper>;
        case Types::INT8: return new List<Int8Wrapper>;
        case Types::INT16: return new List<Int16Wrapper>;
        case Types::INT32: return new List<Int32Wrapper>;
        case Types::INT64: return new List<Int64Wrapper>;
        case Types::UINT8: return new List<Uint8Wrapper>;
        case Types::UINT16: return new List<Uint16Wrapper>;
        case Types::UINT32: return new List<Uint32Wrapper>;
        case Types::UINT64: return new List<Uint64Wrapper>;
        case Types::FLOAT32: return new List<Float32Wrapper>;
        case Types::FLOAT64: return new List<Float64Wrapper>;
        case Types::STRING: return new List<String>;
        case Types::DYNMAP: return new List<DynMap>;
        default:;
        }
    }
    else if (typeInt & (int16_t)Types::MAP_OF) {
        type = (Types)(typeInt & ~(int16_t)Types::MAP_OF);
        switch(type) {
        case Types::BOOL: return new Map<BoolWrapper>;
        case Types::INT8: return new Map<Int8Wrapper>;
        case Types::INT16: return new Map<Int16Wrapper>;
        case Types::INT32: return new Map<Int32Wrapper>;
        case Types::INT64: return new Map<Int64Wrapper>;
        case Types::UINT8: return new Map<Uint8Wrapper>;
        case Types::UINT16: return new Map<Uint16Wrapper>;
        case Types::UINT32: return new Map<Uint32Wrapper>;
        case Types::UINT64: return new Map<Uint64Wrapper>;
        case Types::FLOAT32: return new Map<Float32Wrapper>;
        case Types::FLOAT64: return new Map<Float64Wrapper>;
        case Types::STRING: return new Map<String>;
        case Types::DYNMAP: return new Map<DynMap>;
        default:;
        }
    }
    return NULL;
}
