#ifndef COMMON_LOGGING_H
#define COMMON_LOGGING_H

#include <map>
#include <vector>
#include <string>
#include <cstdarg>
#include <fstream>

#include "common/singleton.h"

#define LOG_FUNCTION_TEMPLATE(_name_, _level_, _prefix_) \
    inline void _name_(char const *format, ...) { \
        if (_level_ > m_level) return; \
        va_list vl; \
        va_start(vl, format); \
        writeEntry(_prefix_, format, vl); \
        va_end(vl); \
    }

namespace Ponyca {
    namespace Log {
        enum Level {
            CRITICAL,
            ERROR,
            WARNING,
            INFO,
            DEBUG
        };

        class AbstractBackend {
        public:
            virtual void write(std::string const &data) = 0;
        };

        class FileBackend : public AbstractBackend {
        public:
            FileBackend(char const *file);
            virtual void write(std::string const &data);
        private:
            std::ofstream m_fs;
        };

        class CoutBackend : public AbstractBackend {
        public:
            virtual void write(std::string const &data);
        };

        class CerrBackend : public AbstractBackend {
        public:
            virtual void write(std::string const &data);
        };
    }

    class Logger {
    public:
        static const unsigned int DefaultBufferSize = 1024;
        static const unsigned int TimeBufferSize = 9;

        inline uint getBufferSize() const {
            return m_bufferSize;
        }
        inline void setBufferSize(uint buffsize) {
            m_bufferSize = buffsize;
        }
        inline Log::Level getLevel() const {
            return m_level;
        }
        inline void setLevel(Log::Level l) {
            m_level = l;
        }

        void addBackend(std::shared_ptr<Log::AbstractBackend> backend);
        void delBackend(std::shared_ptr<Log::AbstractBackend> backend);
        void logRaw(std::string const &data);

        LOG_FUNCTION_TEMPLATE(logCrit,  Log::Level::CRITICAL, "CRIT ")
        LOG_FUNCTION_TEMPLATE(logError, Log::Level::ERROR,    "ERROR")
        LOG_FUNCTION_TEMPLATE(logInfo,  Log::Level::INFO,     "INFO ")
        LOG_FUNCTION_TEMPLATE(logWarn,  Log::Level::WARNING,  "WARN ")
        LOG_FUNCTION_TEMPLATE(logDebug, Log::Level::DEBUG,    "DEBUG")


        static Logger &GetLogger(std::string const &key) {
            if (m_Loggers.find(key) == m_Loggers.end()) {
                m_Loggers[key] = new Logger(key);
            }
            return *m_Loggers[key];
        }

        static void SetLogger(std::string const &key, Logger& logger) {
            m_Loggers[key] = &logger;
        }

        static void DeleteLogger(std::string const &key) {
            m_Loggers.erase(key);
        }

        static void DeleteAllLoggers() {
            auto it = m_Loggers.begin();
            for (; it != m_Loggers.end(); it++) {
                delete it->second;
            }
            m_Loggers.clear();
        }


    private:
        Logger(std::string name)
            : m_name(name)
            , m_bufferSize(DefaultBufferSize)
            , m_level(Log::INFO)
        {}

        inline void makeTime(char *buffer) {
            time_t rawtime;
            struct tm * timeinfo;

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buffer, TimeBufferSize, "%H:%M:%S", timeinfo);
        }

        inline void write(std::string const &data) {
            auto it = m_backends.begin();
            for (; it != m_backends.end(); it++) {
                (*it)->write(data);
            }
        }

        inline void writeEntry(char const *levelPrefix, char const *msg) {
            char* timeString = new char[TimeBufferSize];
            makeTime(timeString);

            std::string buffer;
            buffer += timeString;
            buffer += ' ';
            buffer += m_name;
            buffer += '/';
            buffer += levelPrefix;
            buffer += ": ";
            buffer += msg;
            write(buffer);

            delete[] timeString;
        }

        inline void writeEntry(char const *levelPrefix, char const *format, va_list vl) {
            char *msg = new char[m_bufferSize];
            vsprintf(msg, format, vl);
            writeEntry(levelPrefix, msg);
            delete[] msg;
        }

        Log::Level      m_level;
        unsigned int    m_bufferSize;
        std::string     m_name;
        std::vector< std::shared_ptr<Log::AbstractBackend> > m_backends;
        static std::map<std::string, Logger*> m_Loggers;
    };

    extern Logger &coreLog;
}

#undef LOG_FUNCTION_TEMPLATE

#endif // COMMON_LOGGING_H
