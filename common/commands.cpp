#include "common/commands.h"
#include "common/config.h"

using namespace Ponyca;

void Ponyca::registerCommonCommands(CommandParser &parser) {
    parser.registerCommand(&Commands::echo, "echo", "simple echo");
    parser.registerCommand(&Commands::set, "set", "set <cvar> [value] : manage cvars");
    parser.registerCommand(&Commands::help, "help", "help [cvar|command] : print help");
    parser.registerCommand(&Commands::quit, "quit", "quit.");
}


void Commands::echo(CommandParser::Context &ctx) {
    auto it = ctx.args.begin();
    std::string output;
    while(it != ctx.args.end()) {
        output += *it + " ";
        it++;
    }
    ctx.player.sendMessage(output);
}

void Commands::set(CommandParser::Context &ctx) {
    if (ctx.args.size() < 1 || ctx.args.size() > 2) {
        throw CommandParser::InvalidArgumentsError();
    }
    ConfigManager &mgr = ConfigManager::GetInstance();
    if (ctx.args.size() == 1) {
        std::string var = ctx.args[0];
        if (!mgr.hasKey(var)) {
            ctx.player.sendMessage(var + " does not exists");
            return;
        }
        std::string val = ConfigManager::GetInstance()[var].value;
        ctx.player.sendMessage(var + " = " + val);
    } else {
        std::string var = ctx.args[0];
        std::string val = ctx.args[1];
        ConfigManager::GetInstance()[var].value = val;
    }
}

void Commands::help(CommandParser::Context &ctx) {
    if (ctx.args.size() > 0) {
        // FIXME: search command and print its helptext
        // return;
    }
    auto it = ctx.parser.getCommandMap().begin();
    for(; it!=ctx.parser.getCommandMap().end(); it++) {
        const CommandParser::Command &cmd = it->second;
        ctx.player.sendMessage(cmd.name + ": " + cmd.helpText);
    }
}

void Commands::quit(CommandParser::Context &ctx) {
    throw CommandParser::QuitError();
}

