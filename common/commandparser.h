#ifndef COMMON_COMMANDPARSER_H
#define COMMON_COMMANDPARSER_H

#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <stdexcept>

#include "common/world/world.h"
#include "common/net/network.h"
#include "common/player.h"

namespace Ponyca {
    class CommandParser {
    public:
        struct Context;
        typedef std::vector<std::string> ArgList;
        typedef void (*CommandFnPtr)(Context& ctx);

        struct Command {
            std::string name;
            std::string helpText;
            CommandFnPtr function;
        };

        struct Context {
            Context(std::string const &input_, CommandParser &parser_, AbstractPlayer &player_);

            std::string inputCommand;
            Command parsedCommand;
            ArgList args;
            CommandParser &parser;
            AbstractPlayer &player;
        };

        class NotFoundError : public std::runtime_error {
        public:
            NotFoundError(const std::string& cmd)
                : runtime_error("command not found: "+cmd)
                , m_command(cmd)
            {}
            virtual ~NotFoundError() throw() {}

            inline std::string& getCommand() {
                return m_command;
            }

        private:
            std::string m_command;
        };

        class InvalidArgumentsError : public std::runtime_error {
        public:
            InvalidArgumentsError()
                : runtime_error("command called with invalid arguments")
            {}
        };

        class QuitError : public std::runtime_error {
        public:
            QuitError()
                : runtime_error("shutting down server")
            {}
        };

        CommandParser();
        ~CommandParser();

        bool run(std::string const &line);
        bool run(std::string const &line, AbstractPlayer &sender);
        bool runFile(std::string const &filename);
        bool runFile(std::string const &filename, AbstractPlayer &sender);

        bool registerCommand(CommandFnPtr fn, std::string const &cmd, std::string const &helpText);
        void removeCommand(std::string const &cmd);
        Command& getCommand(std::string const &cmd);

        inline const std::string& getLastError() const {
            return m_lastError;
        }
        inline const std::map<std::string, Command> &getCommandMap() const {
            return m_commands;
        }

    private:
        std::string m_lastError;
        std::map<std::string, Command> m_commands;
    };
}

#endif // COMMON_COMMANDPARSER_H
