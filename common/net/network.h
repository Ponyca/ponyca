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
            virtual uint32_t unserialize(std::string const &buffer) = 0;
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
