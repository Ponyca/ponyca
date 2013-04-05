#ifndef COMMON_PLAYER_H
#define COMMON_PLAYER_H

#include "common/pch.h"
#include "common/logging.h"

namespace Ponyca {
    class AbstractPlayer {
    public:
        AbstractPlayer(std::string const &username);
        virtual ~AbstractPlayer() {}

        inline std::string const &getUsername() const {
            return m_username;
        }
        inline uint32_t getEntityId() const {
            return m_entityId;
        }

        // Send message to player. Overriden by LocalPlayer and RemotePlayer
        virtual void sendMessage(std::string const &message) = 0;

    private:
        std::string m_username;
        uint32_t m_entityId; // if any, else 0.
    };

    class ConsolePlayer : public AbstractPlayer {
    public:
        ConsolePlayer();
        virtual void sendMessage(const std::string &message);
    };
}

#endif // COMMON_PLAYER_H
