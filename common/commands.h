#ifndef COMMON_COMMANDS_H
#define COMMON_COMMANDS_H

#include "common/pch.h"
#include "common/commandparser.h"

namespace Ponyca {
    void registerCommonCommands(CommandParser &parser);

    namespace Commands {
        void echo(CommandParser::Context &ctx);
        void set(CommandParser::Context &ctx);
        void help(CommandParser::Context &ctx);
        void quit(CommandParser::Context &ctx);
    }
}

#endif // COMMON_COMMANDS_H
