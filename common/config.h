#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <string>
#include <map>

#include "singleton.h"

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

            ValueType &operator=(bool b);
            ValueType &operator=(int32_t i);
            ValueType &operator=(float f);
            ValueType &operator=(std::string const &s);
            ValueType &operator=(char const *s);
        };

        struct ValueContainer {
            ValueType value;
            ValueType defaultValue;
            std::string helpText;
        };

        ConfigManager() {}
        ValueContainer &operator[](std::string const &key);

        void loadFile(std::string const &path);

    protected:
        std::map<std::string, ValueContainer> m_map;
    };

    ConfigManager::ValueType getConfigValue(std::string const &key);
}

#endif // COMMON_CONFIG_H
