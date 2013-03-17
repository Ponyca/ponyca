#ifndef COMMON_NET_NETWORK_H
#define COMMON_NET_NETWORK_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

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
            virtual uint16_t unserialize(char const * buffer) = 0;

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
            std::string serializeString(std::string const &v) const;
            template<typename T>
            std::string serializeList(std::vector<T> const &v) const;

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
            uint16_t unserializeString(char const *buffer, std::string &member) const;
            template<typename T>
            uint16_t unserializeList(char const *buffer, std::vector<T> &member) const;
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
    }
}

#endif // COMMON_NET_NETWORK_H
