#include <iostream>
#include "common/net/server.h"
#include "common/config.h"
#include "common/plugins.h"
#include "common/logging.h"
#include "common/commandparser.h"
#include "build/opcodes.h"

using namespace Ponyca;

int main(int argc, char** argv) {
    try {
        // Set up logging
        Logger::SetLogger("core", coreLog);
        coreLog.addBackend(std::shared_ptr<Log::AbstractBackend>(new Log::CoutBackend()));
        coreLog.setLevel(Log::DEBUG);
        coreLog.logRaw("-- Hello Equestria! --");
        coreLog.logInfo("Starting server");

        CommandParser parser;

        // Run autoexec.cfg (if any)
        parser.runFile("./autoexec.cfg");

        PluginsInterface pluginsInterface;

        // Run CLI args
        std::string command;
        for (int i=1; i<argc; i++) {
            if (argv[i][0] == '+') {
                parser.run(command);
            } else {
                command += argv[i];
                command += " ";
            }
        }

        // Initialize everything
        Net::ServerRouter sr;
        sr.listen();

        // Listen for console commands
        try {
            std::string input;
            while(true && std::cin.good()) {
                std::cout << "> " << std::flush;
                std::getline(std::cin, input);
                parser.run(input);
            }
        }
        catch (CommandParser::QuitError) {}

        // Close everything, exit.
        coreLog.logRaw("-- exiting --");
        sr.close();
        Logger::DeleteAllLoggers();
    }
    catch (std::exception &e) {
        std::cerr << "**** EXCEPTION ****" << std::endl;
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
