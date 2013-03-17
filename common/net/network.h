#ifndef COMMON_NET_NETWORK_H
#define COMMON_NET_NETWORK_H

#include <cstdint>
#include <stdexcept>
#include <string>

namespace Ponyca {
    namespace Net {

        class BadPacketError : public std::runtime_error {
        public:
            BadPacketError(uint16_t opcode)
                : runtime_error("bad packet with opcode "+std::to_string(opcode))
            {}
        };

        class AbstractSerializable {
        public:
            virtual std::string serialize() const = 0;
            virtual uint16_t unserialize(std::string const &buffer) = 0;

        protected:
            std::string serializeBool(bool v) const;
            std::string serializeInt8(int8_t v) const;
            std::string serializeInt16(int16_t v) const;
            std::string serializeInt32(int32_t v) const;
            std::string serializeInt64(int64_t v) const;
            std::string serializeUint8(uint8_t v) const;
            std::string serializeUint16(uint16_t v) const;
            std::string serializeUint32(uint32_t v) const;
            std::string serializeUint64(uint64_t v) const;
            std::string serializeFloat32(float v) const;
            std::string serializeFloat64(double v) const;

            uint16_t unserializeBool(char const *buffer, bool &member) const;
            uint16_t unserializeInt8(char const *buffer, int8_t &member) const;
            uint16_t unserializeInt16(char const *buffer, int16_t &member) const;
            uint16_t unserializeInt32(char const *buffer, int32_t &member) const;
            uint16_t unserializeInt64(char const *buffer, int64_t &member) const;
            uint16_t unserializeUint8(char const *buffer, uint8_t &member) const;
            uint16_t unserializeUint16(char const *buffer, uint16_t &member) const;
            uint16_t unserializeUint32(char const *buffer, uint32_t &member) const;
            uint16_t unserializeUint64(char const *buffer, uint64_t &member) const;
            uint16_t unserializeFloat32(char const *buffer, float &member) const;
            uint16_t unserializeFloat64(char const *buffer, double &member) const;
        };

        class StringType : public AbstractSerializable {
        public:
            virtual std::string serialize() const;
            virtual uint16_t unserialize(std::string const &buffer);

            uint16_t length;
            std::wstring string;
        };

        class AbstractPacket : public AbstractSerializable {
        public:
            static const uint16_t Opcode;
        };

        class AbstractRouter {
        public:
            virtual void send(AbstractPacket const &packet, int32_t sessid) = 0;
            // receive
        };
    }
}



#endif // COMMON_NET_NETWORK_H
