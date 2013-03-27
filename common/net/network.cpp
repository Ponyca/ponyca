#include <cstring>

#include "network.h"

using namespace Ponyca::Net;

void AbstractSerializable::setBufferEnd(char const *endptr) {
    m_unserializeBufferEnd = endptr;
}

void AbstractSerializable::checkEndBuffer(char const *ptr) const {
    if (ptr >= m_unserializeBufferEnd) {
        throw UnserializeError();
    }
}

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


uint16_t AbstractSerializable::unserializeBool(char const *buffer, bool &member) const {
    checkEndBuffer(buffer);
    member = buffer[0] != 0;
    return 1;
}

uint16_t AbstractSerializable::unserializeInt8(char const *buffer, int8_t &member) const {
    checkEndBuffer(buffer);
    memcpy(&member, buffer, 1);
    return 1;
}

uint16_t AbstractSerializable::unserializeInt16(char const *buffer, int16_t &member) const {
    checkEndBuffer(buffer+1);
    memcpy(&member, buffer, 2);
    return 2;
}

uint16_t AbstractSerializable::unserializeInt32(char const *buffer, int32_t &member) const {
    checkEndBuffer(buffer+3);
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeInt64(char const *buffer, int64_t &member) const {
    checkEndBuffer(buffer+7);
    memcpy(&member, buffer, 8);
    return 8;
}

uint16_t AbstractSerializable::unserializeUint8(char const *buffer, uint8_t &member) const {
    checkEndBuffer(buffer);
    memcpy(&member, buffer, 1);
    return 1;
}

uint16_t AbstractSerializable::unserializeUint16(char const *buffer, uint16_t &member) const {
    checkEndBuffer(buffer+1);
    memcpy(&member, buffer, 2);
    return 2;
}

uint16_t AbstractSerializable::unserializeUint32(char const *buffer, uint32_t &member) const {
    checkEndBuffer(buffer+3);
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeUint64(char const *buffer, uint64_t &member) const {
    checkEndBuffer(buffer+7);
    memcpy(&member, buffer, 8);
    return 8;
}

uint16_t AbstractSerializable::unserializeFloat32(char const *buffer, float &member) const {
    checkEndBuffer(buffer+3);
    memcpy(&member, buffer, 4);
    return 4;
}

uint16_t AbstractSerializable::unserializeFloat64(char const *buffer, double &member) const {
    checkEndBuffer(buffer+7);
    memcpy(&member, buffer, 8);
    return 8;
}

uint16_t AbstractSerializable::unserializeString(char const *buffer, std::string &member) const {
    uint16_t size;
    unserializeUint16(buffer, size);
    checkEndBuffer(buffer+2+size);
    member.assign(buffer+2, size);

    return size+2;
}


std::string TCPPacketHeader::serialize() const {
    std::string buffer;
    buffer += serializeUint16(opcode);
    buffer += serializeUint16(length);
    return buffer;
}

uint16_t TCPPacketHeader::unserialize(const char *buffer) {
    uint16_t offset = 0;
    offset += unserializeUint16(buffer+offset, opcode);
    offset += unserializeUint16(buffer+offset, length);
    return offset;
}

