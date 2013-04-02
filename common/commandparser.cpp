#include <fstream>

#include "common/commandparser.h"
#include "common/logging.h"
#include "common/config.h"
#include "common/stringutils.h"
#include "common/commands.h"

using namespace Ponyca;

CommandParser::Context::Context(const std::string &input_,
                                CommandParser &parser_, AbstractPlayer &player_)
    : parser(parser_)
    , player(player_)
{
    size_t cmdendpos = input_.find_first_of(" \t");
    args.clear();
    if(cmdendpos == std::string::npos) {
        // only command, it take full string length
        cmdendpos = input_.length();
    } else {
        // there are args, parse them
        size_t pos, lastpos;
        lastpos = cmdendpos+1;

        while(pos=input_.find_first_of(" \t\n", lastpos)) {
            if (pos == std::string::npos) {
                pos = input_.length();
            }
            if (lastpos > pos) {
                break;
            }
            std::string meow = input_.substr(lastpos, pos-lastpos);
            if (!meow.empty()) {
                args.push_back(meow);
            }
            lastpos = pos+1;
        }
    }
    if (input_.empty()) {
        inputCommand = "";
        parsedCommand.function = NULL;
        return;
    }

    inputCommand = input_.substr(0, cmdendpos);
    parsedCommand = parser.getCommand(inputCommand);
}


CommandParser::CommandParser() {
    registerCommonCommands(*this);
}

CommandParser::~CommandParser() {

}

bool CommandParser::run(std::string const &line) {
    ConsolePlayer p;
    run(line, p);
}

bool CommandParser::run(std::string const &line, AbstractPlayer &player) {
    try {
        Context ctx = Context(line, *this, player);
        try {
            if (ctx.parsedCommand.function != NULL) {
                (ctx.parsedCommand.function)(ctx);
            }
        }
        catch(InvalidArgumentsError& e) {
            // display help string
            std::string errorMessage = ctx.inputCommand + ": ";
            errorMessage.append(ctx.parsedCommand.helpText);
            player.sendMessage(errorMessage);
            return false;
        }
        return true;
    }
    catch(NotFoundError& e) {
        // is the command a cvar ? print it.
        ConfigManager &mgr = ConfigManager::GetInstance();
        if (mgr.hasKey(e.getCommand())) {
            std::string value = getConfigValue(e.getCommand()).string;
            player.sendMessage(e.getCommand()+" = "+value);
        } else {
            player.sendMessage(e.what());
        }
        return false;
    }
}

bool CommandParser::runFile(std::string const &filename) {
    ConsolePlayer p;
    runFile(filename, p);
}

bool CommandParser::runFile(std::string const &filename, AbstractPlayer &player) {
    std::ifstream fs(filename.c_str(), std::ios::in);
    if (!fs.good()) {
        return false;
    }

    bool blockComment = false;
    int lineNumber = 0;
    size_t pos;
    std::string line;

    while(std::getline(fs, line)) {
        lineNumber++;

        if (blockComment) {
            if ((pos = line.find("*/")) != std::string::npos) {
                line.substr(pos+2, 0);
                if (line.length() < 1) {
                    // only "*/" on the line
                    continue;
                }
            } else {
                // ignore line
                continue;
            }
        }

        // Remove comments
        if ((pos = line.find("//")) != std::string::npos) {
            line.resize(pos);
        }
        if ((pos = line.find_first_of("#")) != std::string::npos) {
            line.resize(pos);
        }

        // Is empty ?
        StringUtils::trim(line);
        if (line.empty()) {
            continue;
        }

        // Multiple commands on one line
        if ((pos = line.find(";")) != std::string::npos) {
            line.resize(pos);
        }

        size_t offset = 0;
        do {
            pos = line.find(";");
            std::string part = line.substr(offset, pos==std::string::npos?line.length():pos);
            run(part, player);
        } while(pos != std::string::npos);
    }
    return true;
}

bool CommandParser::registerCommand(CommandFnPtr fn, std::string const &cmd, std::string const &helpText) {
    removeCommand(cmd); // Delete if already registered
    Command& cmdMetas = m_commands[cmd];
    cmdMetas.name = cmd;
    cmdMetas.helpText = helpText;
    cmdMetas.function = fn;
    return true;
}

void CommandParser::removeCommand(std::string const &cmd) {
    m_commands.erase(cmd);
}

CommandParser::Command& CommandParser::getCommand(std::string const &cmd) {
    auto it = m_commands.find(cmd);
    if(it != m_commands.end()) {
        return it->second;
    }
    throw NotFoundError(cmd);
}


