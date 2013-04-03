#include "common/player.h"

using namespace Ponyca;

AbstractPlayer::AbstractPlayer(const std::string &username)
    : m_username(username)
{}


ConsolePlayer::ConsolePlayer()
    : AbstractPlayer("Console")
{}

void ConsolePlayer::sendMessage(const std::string &message) {
    coreLog.logInfo("Message: %s", message.c_str());
}


