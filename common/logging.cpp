#include "common/pch.h"
#include "common/logging.h"

using namespace Ponyca;

std::map<std::string, Logger*> Logger::m_Loggers;
Logger &Ponyca::coreLog = Logger::GetLogger("core");

Log::FileBackend::FileBackend(char const *file)
    : m_fs(file, std::ios::app)
{}

void Log::FileBackend::write(std::string const &data) {
    m_fs << data << std::endl;
}


void Log::CoutBackend::write(const std::string &data) {
    std::cout << data << std::endl;
}


void Log::CerrBackend::write(const std::string &data) {
    std::cerr << data << std::endl;
}

void Logger::addBackend(std::shared_ptr<Log::AbstractBackend> backend) {
    m_backends.push_back(std::shared_ptr<Log::AbstractBackend>(backend));
}

void Logger::delBackend(std::shared_ptr<Log::AbstractBackend> backend) {
    auto it = m_backends.begin();
    for (; it != m_backends.end(); it++) {
        if ((*it) == backend) {
            m_backends.erase(it);
            return;
        }
    }
}

void Logger::logRaw(std::string const &data) {
    write(data);
}

