#include <cstring>

#include "network.h"


using namespace Ponyca::Net;

std::string AbstractSerializable::serializeBool(bool v) const {
    if (v) {
        return std::string("\0");
    }
    else {
        return std::string("\1");
    }
}

std::string AbstractSerializable::serializeInt8(int8_t v) const {
    return std::string((char*)(&v), 1);
}

std::string AbstractSerializable::serializeInt16(int16_t v) const {
    return std::string((char*)(&v), 2);
}

std::string AbstractSerializable::serializeInt32(int32_t v) const {
    return std::string((char*)(&v), 4);
}

std::string AbstractSerializable::serializeInt64(int64_t v) const {
    return std::string((char*)(&v), 8);
}

std::string AbstractSerializable::serializeUInt8(uint8_t v) const {
    return std::string((char*)(&v), 1);
}

std::string AbstractSerializable::serializeUInt16(uint16_t v) const {
    return std::string((char*)(&v), 2);
}

std::string AbstractSerializable::serializeUInt32(uint32_t v) const {
    return std::string((char*)(&v), 4);
}

std::string AbstractSerializable::serializeUInt64(uint64_t v) const {
    return std::string((char*)(&v), 8);
}

std::string AbstractSerializable::serializeFloat32(float v) const {
    return std::string((char*)(&v), 4);
}

std::string AbstractSerializable::serializeFloat64(double v) const {
    return std::string((char*)(&v), 8);
}


uint16_t AbstractSerializable::unserializeBool(char const *buffer, bool &member) const {
    member = buffer[0] != 0;
    return 1;
}

uint16_t AbstractSerializable::unserializeInt8(char const *buffer, int8_t &member) const {
    memcpy(&member, buffer, 1);
    return 1;
}

uint16_t AbstractSerializable::unserializeInt16(char const *buffer, int16_t &member) const {
    memcpy(&member, buffer, 2);
    return 2;
}

uint16_t AbstractSerializable::unserializeInt32(char const *buffer, int32_t &member) const {
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeInt64(char const *buffer, int64_t &member) const {
    memcpy(&member, buffer, 8);
    return 8;
}

uint16_t AbstractSerializable::unserializeUInt8(char const *buffer, uint8_t &member) const {
    memcpy(&member, buffer, 1);
    return 1;
}

uint16_t AbstractSerializable::unserializeUInt16(char const *buffer, uint16_t &member) const {
    memcpy(&member, buffer, 2);
    return 2;
}

uint16_t AbstractSerializable::unserializeUInt32(char const *buffer, uint32_t &member) const {
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeUInt64(char const *buffer, uint64_t &member) const {
    memcpy(&member, buffer, 8);
    return 8;
}

uint16_t AbstractSerializable::unserializeFloat32(char const *buffer, float &member) const {
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeFloat64(char const *buffer, double &member) const {
    memcpy(&member, buffer, 8);
    return 8;
}




