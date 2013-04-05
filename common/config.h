#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include "common/pch.h"
#include "common/singleton.h"

namespace Ponyca {
    class ConfigManager : public Singleton<ConfigManager> {
    public:
        struct ValueType {
            bool boolean;
            int32_t integer;
            float floating;
            std::string string;

            operator bool() const;
            operator int32_t() const;
            operator float() const;
            operator std::string() const;
            operator char const *() const;

            inline operator int8_t() const {
                return operator int32_t();
            }
            inline operator uint8_t() const {
                return operator int32_t();
            }
            inline operator int16_t() const {
                return operator int32_t();
            }
            inline operator uint16_t() const {
                return operator int32_t();
            }
            inline operator uint32_t() const {
                return operator int32_t();
            }
            inline operator int64_t() const {
                return operator int32_t();
            }
            inline operator uint64_t() const {
                return operator int32_t();
            }

            ValueType &operator=(bool b);
            ValueType &operator=(int32_t i);
            ValueType &operator=(float f);
            ValueType &operator=(std::string const &s);
            ValueType &operator=(char const *s);

            inline ValueType &operator=(int8_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(uint8_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(int16_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(uint16_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(uint32_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(int64_t i) {
                return operator =((int32_t)i);
            }
            inline ValueType &operator=(uint64_t i) {
                return operator =((int32_t)i);
            }
        };

        struct ValueContainer {
            ValueType value;
            ValueType defaultValue;
            std::string helpText;

            template<typename T>
            void define(T v, char const *help=NULL) {
                value = v;
                defaultValue = v;
                helpText = help;
            }
        };

        ConfigManager() {}
        ValueContainer &operator[](std::string const &key);
        bool hasKey(std::string const &key) const;

        void loadFile(std::string const &path);

    protected:
        std::map<std::string, ValueContainer> m_map;
    };

    ConfigManager::ValueType getConfigValue(std::string const &key);
}

#endif // COMMON_CONFIG_H
