#include "common/pch.h"
#include "common/config.h"
#include "common/stringutils.h"

using namespace Ponyca;

ConfigManager::ValueType::operator bool() const {
    return boolean;
}

ConfigManager::ValueType::operator int32_t() const {
    return integer;
}

ConfigManager::ValueType::operator float() const {
    return floating;
}

ConfigManager::ValueType::operator std::string() const {
    return string;
}

ConfigManager::ValueType::operator char const *() const {
    return string.c_str();
}


ConfigManager::ValueType& ConfigManager::ValueType::operator=(bool b) {
    boolean = b;
    integer = b?1:0;
    floating = b?1.0:0;
    string = b?"true":"false";
    return *this;
}

ConfigManager::ValueType& ConfigManager::ValueType::operator=(int32_t i) {
    boolean = i != 0;
    integer = i;
    floating = i;
    string = std::to_string(i);
    return *this;
}

ConfigManager::ValueType& ConfigManager::ValueType::operator=(float f) {
    boolean = f != 0;
    integer = floor(f);
    floating = f;
    string = std::to_string(f);
    return *this;
}

ConfigManager::ValueType& ConfigManager::ValueType::operator=(std::string const &s) {
    boolean = s.length() != 0;

    try {
        integer = std::stoi(s);
    }
    catch (std::exception) {
        integer = 0;
    }

    try {
        floating = std::stof(s);
    }
    catch (std::exception) {
        floating = 0;
    }

    string = s;
    return *this;
}

ConfigManager::ValueType& ConfigManager::ValueType::operator=(char const *s) {
    return operator=(std::string(s));
}


ConfigManager::ValueContainer &ConfigManager::operator[](std::string const &key) {
    return m_map[key];
}

void ConfigManager::loadFile(std::string const &path) {
    std::ifstream fs(path, std::ios::in);
    if (!fs.good()) {
        throw std::runtime_error("cannot read "+path);
    }
    int lineNumber = 0;
    size_t pos;
    std::string line, key, value, prefix("");
    while(std::getline(fs, line)) {
        lineNumber++;

        // Remove comments
        if ((pos = line.find("//")) != std::string::npos) {
            line.resize(pos);
        }
        if ((pos = line.find_first_of("#;")) != std::string::npos) {
            line.resize(pos);
        }

        // Is empty ?
        StringUtils::trim(line);
        if (line.empty()) {
            continue;
        }

        // Namespace ?
        if (line[0] == '[' && (pos=line.find(']')) != std::string::npos) {
            if (pos == 1) {
                prefix = "";
                continue;
            }
            prefix = line.substr(1, pos-1);
            StringUtils::trim(prefix);
            prefix.push_back('.');
            continue;
        }

        // Find the "="
        pos = line.find("=");
        if(pos == std::string::npos) {
            // Empty var, ignore
            continue;
        }
        key   = line.substr(0, pos);
        value = line.substr(pos+1);
        StringUtils::trim(key);
        StringUtils::trim(value);

        if (((value.front() == '"' && value.back() == '"')
             || (value.front() == '\'' && value.back() == '\''))
         && value.length() >= 3) {
            // Remove quotes
            value = value.substr(1, -1);
        }

        if (key.empty()) {
            // Empty key, ignore it
            continue;
        }
        key = prefix+key;

        m_map[key].value = value;
    }
}

bool ConfigManager::hasKey(std::string const &key) const {
    return m_map.find(key) != m_map.end();
}

ConfigManager::ValueType Ponyca::getConfigValue(std::string const &key) {
    return ConfigManager::GetInstance()[key].value;
}
