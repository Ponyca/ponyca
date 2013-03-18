#include <cstring>

#include "network.h"



using namespace Ponyca::Net;

std::string AbstractSerializable::serializeBool(bool v) const {
    return std::string(v ? "\1" : "\0");
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

std::string AbstractSerializable::serializeUint8(uint8_t v) const {
    return std::string((char*)(&v), 1);
}

std::string AbstractSerializable::serializeUint16(uint16_t v) const {
    return std::string((char*)(&v), 2);
}

std::string AbstractSerializable::serializeUint32(uint32_t v) const {
    return std::string((char*)(&v), 4);
}

std::string AbstractSerializable::serializeUint64(uint64_t v) const {
    return std::string((char*)(&v), 8);
}

std::string AbstractSerializable::serializeFloat32(float v) const {
    return std::string((char*)(&v), 4);
}

std::string AbstractSerializable::serializeFloat64(double v) const {
    return std::string((char*)(&v), 8);
}

std::string AbstractSerializable::serializeString(std::string const &v) const {
    std::string buffer;
    buffer += serializeUint16(v.size());
    buffer += v;
    return buffer;
}

template<typename T>
std::string AbstractSerializable::serializeList(std::vector<T> const &v) const {
    std::string buffer;
    buffer += serializeUint16(v.size());

    typename std::vector<T>::const_iterator it;
    for(it=v.begin(); it!=v.end(); it++) {
        std::string serialized(it->serialize());
        buffer += serialized;
    }
    return buffer;
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

uint16_t AbstractSerializable::unserializeUint8(char const *buffer, uint8_t &member) const {
    memcpy(&member, buffer, 1);
    return 1;
}

uint16_t AbstractSerializable::unserializeUint16(char const *buffer, uint16_t &member) const {
    memcpy(&member, buffer, 2);
    return 2;
}

uint16_t AbstractSerializable::unserializeUint32(char const *buffer, uint32_t &member) const {
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeUint64(char const *buffer, uint64_t &member) const {
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

uint16_t AbstractSerializable::unserializeString(char const *buffer, std::string &member) const {
    uint16_t size;
    unserializeUint16(buffer, size);
    member.assign(buffer+2, size);

    return size+2;
}

template<typename T>
uint16_t AbstractSerializable::unserializeList(char const *buffer, std::vector<T> &member) const {
    uint16_t size, i, offset = 0;
    unserializeUint16(buffer, size);

    for(i=0; i<size; i++) {
        T obj;

        offset += obj.unserialize(buffer + 2 + offset);
        member.push_back(obj);
    }

    return 2 + offset;
}
