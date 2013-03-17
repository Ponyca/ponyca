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
<<<<<<< HEAD
            virtual uint16_t unserialize(std::string const &buffer) = 0;

        protected:
            std::string serializeBool(bool v) const;
            std::string serializeInt8(int8_t v) const;
            std::string serializeInt16(int16_t v) const;
            std::string serializeInt32(int32_t v) const;
            std::string serializeInt64(int64_t v) const;
            std::string serializeUInt8(uint8_t v) const;
            std::string serializeUInt16(uint16_t v) const;
            std::string serializeUInt32(uint32_t v) const;
            std::string serializeUInt64(uint64_t v) const;
            std::string serializeFloat32(float v) const;
            std::string serializeFloat64(double v) const;

            uint16_t unserializeBool(char const *buffer, bool &member) const;
            uint16_t unserializeInt8(char const *buffer, int8_t &member) const;
            uint16_t unserializeInt16(char const *buffer, int16_t &member) const;
            uint16_t unserializeInt32(char const *buffer, int32_t &member) const;
            uint16_t unserializeInt64(char const *buffer, int64_t &member) const;
            uint16_t unserializeUInt8(char const *buffer, uint8_t &member) const;
            uint16_t unserializeUInt16(char const *buffer, uint16_t &member) const;
            uint16_t unserializeUInt32(char const *buffer, uint32_t &member) const;
            uint16_t unserializeUInt64(char const *buffer, uint64_t &member) const;
            uint16_t unserializeFloat32(char const *buffer, float &member) const;
            uint16_t unserializeFloat64(char const *buffer, double &member) const;
        };

        class StringType : public AbstractSerializable {
        public:
            virtual std::string serialize() const;
            virtual uint16_t unserialize(std::string const &buffer);

            uint16_t length;
            std::wstring string;
=======
            virtual uint32_t unserialize(std::string const &buffer) = 0;

            static uint64_t unserializeInt32(const char*, int32_t);
            static std::string serializeInt32(int32_t);
            static uint64_t unserializeDirection(const char*, struct direction);
            static std::string serializeDirection(struct direction);
            static uint64_t unserializeInt16(const char*, int16_t);
            static std::string serializeInt16(int16_t);
            static uint64_t unserializeString(const char*, std::string);
            static std::string serializeString(std::string);
            static uint64_t unserializeUint8(const char*, uint8_t);
            static std::string serializeUint8(uint8_t);
            static uint64_t unserializeBool(const char*, bool);
            static std::string serializeBool(bool);
            static uint64_t unserializeInt8(const char*, int8_t);
            static std::string serializeInt8(int8_t);
            static uint64_t unserializeStructure(const char*, AbstractSerializable&);
            static std::string serializeStructure(AbstractSerializable&);
            static uint64_t unserializeUint64(const char*, uint64_t);
            static std::string serializeUint64(uint64_t);
            static uint64_t unserializeMsgpack(const char*, msgpack::unpacked);
            static std::string serializeMsgpack(msgpack::unpacked);
            static uint64_t unserializeFloat64(const char*, double);
            static std::string serializeFloat64(double);
            static uint64_t unserializeList(const char*, std::vector<AbstractSerializable>&);
            static std::string serializeList(std::vector<AbstractSerializable>&);
            static uint64_t unserializeCoordinates(const char*, struct coordinates);
            static std::string serializeCoordinates(struct coordinates);
            static uint64_t unserializeUint16(const char*, uint16_t);
            static std::string serializeUint16(uint16_t);
            static uint64_t unserializeInt64(const char*, int64_t);
            static std::string serializeInt64(int64_t);
            static uint64_t unserializePosition(const char*, struct position);
            static std::string serializePosition(struct position);
            static uint64_t unserializeUint32(const char*, uint32_t);
            static std::string serializeUint32(uint32_t);
            static uint64_t unserializeFloat32(const char*, float);
            static std::string serializeFloat32(float);
>>>>>>> c1ce6dcbc99ee5a6c2af563fe42763741d5fddb8
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
