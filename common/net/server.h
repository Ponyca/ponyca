#ifndef COMMON_NET_SERVER_H
#define COMMON_NET_SERVER_H

#include <asio.hpp>
#include <memory>

#include "common/net/network.h"


namespace Ponyca {
    class Logger;

    namespace Net {
        class ServerRouter;

        class RemoteClient {
        public:
            RemoteClient(asio::io_service &ioService, ServerRouter &server);
            ~RemoteClient();
            void start();
            void close();
            void send(AbstractPacket const &packet);
            void handleReadHeader(asio::error_code const &error);
            void handleReadBody(asio::error_code const &error);
            void handleWrite(asio::error_code const &error);

            void displayError(asio::error_code const &error);

            asio::ip::tcp::socket &getSocket();
            std::string getRemoteAddress() const;

            uint32_t associatedEntity;
            std::string username;

        private:
            ServerRouter &m_server;
            asio::ip::tcp::socket m_socket;
            TCPPacketHeader m_readHeader;
            std::vector<char> m_readBuffer;
            Logger &m_log;
        };

        class ServerRouter : public AbstractRouter {
        public:
            typedef RemoteClient *RemotePtr;
            typedef void (*HandlerType)(ServerRouter &server, RemoteClient &client, AbstractPacket const &packet);

            ServerRouter();
            void close();
            void listen();
            void threadMain();
            void displayError(asio::error_code const &error);

            virtual void send(AbstractPacket const &packet, RemoteClient *ptr);
            virtual void sendByUsername(AbstractPacket const &packet, std::string const &username);
            virtual void sendByAddress(AbstractPacket const &packet, std::string const &address);
            virtual void sendByEntity(AbstractPacket const &packet, uint32_t entityId);
            virtual void sendAll(AbstractPacket const &packet);
            void kill(RemoteClient *ptr);

            void setPacketHandler(uint16_t opcode, HandlerType function);
            HandlerType getPacketHandler(uint16_t opcode) const;

            inline Logger &getLog() {
                return m_log;
            }

            static void nullPacketHandler(ServerRouter &server, RemoteClient &client, AbstractPacket const &packet);

            void handleAccept(asio::error_code const &error, RemoteClient *newClient);

        private:
            void accept();

            HandlerType m_handlers[0xffff];
            std::vector< RemoteClient* > m_clients;
            std::vector< std::shared_ptr<asio::thread> > m_threads;
            asio::io_service m_ioService;
            asio::ip::tcp::acceptor m_acceptor;
            int32_t m_lastSessUID;
            Logger &m_log;
        };

    }
}

#endif // COMMON_NET_SERVER_H
