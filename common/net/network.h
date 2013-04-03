#ifndef COMMON_NET_NETWORK_H
#define COMMON_NET_NETWORK_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>
#include <map>

namespace Ponyca {
    namespace Net {

        class BadPacketError : public std::runtime_error {
        public:
            BadPacketError(uint16_t opcode)
                : runtime_error("bad packet with opcode "+std::to_string(opcode))
            {}
        };

        class UnserializeError : public std::exception {};

        class AbstractSerializable {
        public:
            virtual std::string serialize() const = 0;
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes) = 0;
        };

        enum class Types {
            BOOL    = 0x00,
            INT8    = 0x01,
            INT16   = 0x02,
            INT32   = 0x03,
            INT64   = 0x04,
            UINT8   = 0x05,
            UINT16  = 0x06,
            UINT32  = 0x07,
            UINT64  = 0x08,
            FLOAT32 = 0x09,
            FLOAT64 = 0x0a,
            STRING  = 0x0b,
            DYNMAP  = 0x0c,

            LIST_OF = 0x40,
            MAP_OF  = 0x80
        };

        AbstractSerializable* makeSerializable(int16_t typeInt);

        template<typename T, uint16_t S>
        class PlainTypeWrapper : public AbstractSerializable {
        public:
            PlainTypeWrapper() {}
            PlainTypeWrapper(T v)
                : value(v)
            {}
            virtual std::string serialize() const {
                return std::string((char*)(&value), S);
            }
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes) {
                if (S > availableBytes) {
                    throw UnserializeError();
                }
                memcpy(&value, buffer, S);
                return S;
            }

            T value;

            inline operator T() const { return value; }
            inline T operator=(T v) { return value = v; }
            inline T operator+(T v) const { return value+v; }
            inline T operator-(T v) const { return value-v; }
            inline T operator*(T v) const { return value*v; }
            inline T operator/(T v) const { return value/v; }
            inline T operator%(T v) const { return value%v; }
            inline T operator++() { return ++value; }
            inline T operator--() { return --value; }
            inline T operator++(int) { return value++; }
            inline T operator--(int) { return value--; }
            inline bool operator==(T v) const { return value==v; }
            inline bool operator!=(T v) const { return value!=v; }
            inline bool operator<(T v) const { return value<v; }
            inline bool operator>(T v) const { return value>v; }
            inline bool operator>=(T v) const { return value>=v; }
            inline bool operator<=(T v) const { return value<=v; }
            inline T operator!() const { return !value; }
            inline T operator&&(T v) const { return value&&v; }
            inline T operator||(T v) const { return value||v; }
            inline T operator~() const { return ~value; }
            inline T operator&(T v) const { return value&v; }
            inline T operator|(T v) const { return value|v; }
            inline T operator^(T v) const { return value^v; }
            inline T operator<<(T v) { return value<<v; }
            inline T operator>>(T v) { return value>>v; }
            inline T operator+=(T v) { return value+=v; }
            inline T operator-=(T v) { return value-=v; }
            inline T operator*=(T v) { return value*=v; }
            inline T operator/=(T v) { return value/=v; }
            inline T operator%=(T v) { return value%=v; }
            inline T operator&=(T v) { return value&=v; }
            inline T operator|=(T v) { return value|=v; }
            inline T operator^=(T v) { return value^=v; }
            inline T operator<<=(T v) { return value<<=v; }
            inline T operator>>=(T v) { return value>>=v; }
        };

        typedef PlainTypeWrapper<bool, 1> BoolWrapper;
        typedef PlainTypeWrapper<int8_t, 1> Int8Wrapper;
        typedef PlainTypeWrapper<int16_t, 2> Int16Wrapper;
        typedef PlainTypeWrapper<int32_t, 4> Int32Wrapper;
        typedef PlainTypeWrapper<int64_t, 8> Int64Wrapper;
        typedef PlainTypeWrapper<uint8_t, 1> Uint8Wrapper;
        typedef PlainTypeWrapper<uint16_t, 2> Uint16Wrapper;
        typedef PlainTypeWrapper<uint32_t, 4> Uint32Wrapper;
        typedef PlainTypeWrapper<uint64_t, 8> Uint64Wrapper;
        typedef PlainTypeWrapper<float, 4> Float32Wrapper;
        typedef PlainTypeWrapper<double, 8> Float64Wrapper;

        class String : public std::string, public AbstractSerializable {
        public:
            String() {}
            String(std::string const &str)
                : basic_string(str) {}
            virtual std::string serialize() const {
                std::string buffer;
                buffer += Uint16Wrapper(size()).serialize();

                auto it = begin();
                for(; it!=end(); it++) {
                    Int8Wrapper wrapper(*it);
                    buffer += wrapper.serialize();
                }
                return buffer;
            }
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes) {
                uint16_t i, offset = 0;
                Uint16Wrapper length;
                offset += length.unserialize(buffer, availableBytes-offset);

                for(i=0; i<length.value; i++) {
                    Int8Wrapper obj;
                    offset += obj.unserialize(buffer+offset, availableBytes-offset);
                    push_back((char)obj.value);
                }

                return offset;
            }
        };

        template<typename T>
        class List : public AbstractSerializable {
        public:
            virtual std::string serialize() const {
                std::string buffer;
                buffer += Uint16Wrapper(vector.size()).serialize();

                auto it = vector.begin();
                for(; it!=vector.end(); it++) {
                    buffer += it->serialize();
                }
                return buffer;
            }
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes) {
                uint16_t i, offset = 0;
                Uint16Wrapper length;
                offset += length.unserialize(buffer+offset, availableBytes-offset);

                for(i=0; i<length.value; i++) {
                    T obj;
                    offset += obj.unserialize(buffer+offset, availableBytes-offset);
                    vector.push_back(obj);
                }

                return offset;
            }

            std::vector<T> vector;
        };

        template<typename T>
        class Map : public AbstractSerializable {
        public:
            virtual std::string serialize() const {
                std::string buffer;
                buffer += Uint16Wrapper(map.size()).serialize();
                auto it = map.begin();
                for(; it!=map.end(); it++) {
                    buffer += String(it->first).serialize();
                    buffer += it->second.serialize();
                }
                return buffer;
            }
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes) {
                uint16_t i, offset = 0;
                Uint16Wrapper length;
                offset += length.unserialize(buffer+offset, availableBytes-offset);
                for(i=0; i<length.value; i++) {
                    String key;
                    T obj;
                    offset += key.unserialize(buffer+offset, availableBytes-offset);
                    offset += obj.unserialize(buffer+offset, availableBytes-offset);
                    map[key] = obj;
                }
                return 2 + offset;
            }
            std::map<std::string, T> map;
        };

        class DynMap : public AbstractSerializable {
        public:
            virtual std::string serialize() const;
            virtual uint16_t unserialize(char const *buffer, uint16_t availableBytes);
            std::map<String, AbstractSerializable*> map;
        };

        class AbstractPacket : public AbstractSerializable {
        public:
            AbstractPacket(uint16_t opcode_)
                : opcode(opcode_)
            {}
            const Uint16Wrapper opcode;

        };

        // Implementation generated by opcodes_generator.py
        AbstractPacket* makePacket(int16_t opcode);

        class TCPPacketHeader : public AbstractSerializable {
        public:
            virtual std::string serialize() const;
            virtual uint16_t unserialize(const char *buffer, uint16_t availableBytes);
            Uint16Wrapper opcode;
            Uint16Wrapper length;
        };

        class UDPPacketHeader : public AbstractSerializable {
        public:
            virtual std::string serialize() const;
            virtual uint16_t unserialize(const char *buffer, uint16_t availableBytes);
            Uint16Wrapper opcode;
            Uint16Wrapper length;
        };

        class AbstractRouter {
        public:
            //virtual void send(AbstractPacket const &packet, int32_t sessid) = 0;
            virtual void sendAll(AbstractPacket const &packet) = 0;
        };
    }
}

#endif // COMMON_NET_NETWORK_H
