#ifndef COMMON_NET_SERVER_H
#define COMMON_NET_SERVER_H

#include "common/pch.h"
#include "common/net/network.h"
#include "common/player.h"

namespace Ponyca {
    class Logger;

    namespace Net {
        class ServerRouter;

        class RemotePlayer : public AbstractPlayer {
        public:
            RemotePlayer(asio::io_service &ioService, ServerRouter &server);
            ~RemotePlayer();
            void start();
            void close();
            void send(AbstractPacket const &packet);
            virtual void sendMessage(const std::string &message);
            void handleReadHeader(asio::error_code const &error);
            void handleReadBody(asio::error_code const &error);
            void handleWrite(asio::error_code const &error);

            void displayError(asio::error_code const &error);

            asio::ip::tcp::socket &getSocket();
            std::string getRemoteAddress() const;

        private:
            void writePacket();

            asio::ip::tcp::socket m_socket;
            ServerRouter &m_server;
            Logger &m_log;
            TCPPacketHeader m_readHeader;
            std::vector<char> m_readBuffer;
            std::queue<std::string> m_writeQueue;
        };

        class ServerRouter : public AbstractRouter {
        public:
            typedef RemotePlayer *RemotePtr;
            typedef void (*HandlerType)(ServerRouter &server, RemotePlayer &client, AbstractPacket const &packet);

            ServerRouter();
            void close();
            void listen();
            void threadMain();
            void displayError(asio::error_code const &error);

            virtual void send(AbstractPacket const &packet, RemotePlayer *ptr);
            virtual void sendByUsername(AbstractPacket const &packet, std::string const &username);
            virtual void sendByAddress(AbstractPacket const &packet, std::string const &address);
            virtual void sendByEntity(AbstractPacket const &packet, uint32_t entityId);
            virtual void sendAll(AbstractPacket const &packet);
            void kill(RemotePlayer *ptr);

            void setPacketHandler(uint16_t opcode, HandlerType function);
            HandlerType getPacketHandler(uint16_t opcode) const;

            inline Logger &getLog() {
                return m_log;
            }

            static void nullPacketHandler(ServerRouter &server, RemotePlayer &client, AbstractPacket const &packet);

            void handleAccept(asio::error_code const &error, RemotePlayer *newClient);

        private:
            void accept();

            HandlerType m_handlers[0xffff];
            std::vector< RemotePlayer* > m_clients;
            std::vector< std::shared_ptr<asio::thread> > m_threads;
            asio::io_service m_ioService;
            asio::ip::tcp::acceptor m_acceptor;
            int32_t m_lastSessUID;
            Logger &m_log;
        };

    }
}

#endif // COMMON_NET_SERVER_H
