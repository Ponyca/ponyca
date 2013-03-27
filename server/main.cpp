#include <iostream>
#include "common/net/server.h"
#include "common/config.h"
#include "common/logging.h"

using namespace Ponyca;

int main(int argc, char** argv) {
    Logger::SetLogger("core", coreLog);

    coreLog.addBackend(std::shared_ptr<Log::AbstractBackend>(new Log::CoutBackend()));
    coreLog.setLevel(Log::DEBUG);
    coreLog.logRaw("-- Hello Equestria! --");
    coreLog.logInfo("Starting server");

    try {
        Net::ServerRouter sr;
        sr.listen();

        // TODO: Display a nice CLI
        std::cin.get();

        coreLog.logRaw("-- exiting --");
        sr.close();
    }
    catch (std::exception &e) {
        std::cerr << "**** EXCEPTION ****" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    Logger::DeleteAllLoggers();
    return 0;
}
