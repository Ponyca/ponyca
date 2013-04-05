#include "common/pch.h"
#include "common/net/server.h"
#include "common/config.h"
#include "common/logging.h"
#include "build/opcodes.h"

using namespace Ponyca;
using namespace Ponyca::Net;

RemotePlayer::RemotePlayer(asio::io_service &ioService, ServerRouter &server)
    : AbstractPlayer("Player")
    , m_socket(ioService)
    , m_server(server)
    , m_log(m_server.getLog())
{}

void RemotePlayer::sendMessage(const std::string &message) {
    Packets::Message m(0, message);
    send(m);
}

RemotePlayer::~RemotePlayer() {
    if (m_socket.is_open()) {
        m_socket.cancel();
        m_socket.close();
    }
}

void RemotePlayer::start() {
    m_readBuffer.clear();
    m_readBuffer.resize(4);
    auto fn = std::bind(&RemotePlayer::handleReadHeader, this,
                        std::placeholders::_1);
    auto buffer = asio::buffer(m_readBuffer, 4);
    asio::async_read(m_socket, buffer, fn);
}

void RemotePlayer::send(AbstractPacket const &packet) {
    TCPPacketHeader header;
    std::string serialized = packet.serialize();
    header.opcode = packet.opcode;
    header.length = serialized.length();
    serialized.insert(0, header.serialize());
    m_writeQueue.push(serialized);
    writePacket();
}

void RemotePlayer::writePacket() {
    auto buffer = asio::buffer(m_writeQueue.back().c_str(), m_writeQueue.back().length());
    auto fn = std::bind(&RemotePlayer::handleWrite, this,
                        std::placeholders::_1);
    asio::async_write(m_socket, buffer, fn);
}

void RemotePlayer::handleReadHeader(asio::error_code const &error) {
    if (error) {
        displayError(error);
        return;
    }

    if (m_readBuffer.size() != 4) {
        m_log.logDebug("[%s] Invalid packet with %d bytes header",
                       getRemoteAddress().c_str(),
                       m_readBuffer.size());
        m_server.kill(this);
        return;
    }

    m_readHeader.unserialize(m_readBuffer.data(), m_readBuffer.size());

    m_log.logDebug("[%s] Got header: 0x%04x with %d bytes body.",
                   getRemoteAddress().c_str(),
                   m_readHeader.opcode.value,
                   m_readHeader.length.value);

    if (m_readHeader.length.value == 0) {
        // we do not expect a body
        AbstractPacket *packet = makePacket(m_readHeader.opcode);
        try {
            ServerRouter::HandlerType fn;
            fn = m_server.getPacketHandler(m_readHeader.opcode);
            fn(m_server, *this, *packet);
        }
        catch (UnserializeError) {
            m_log.logDebug("[%s] Invalid 0x%04x packet without body",
                           getRemoteAddress().c_str(),
                           m_readHeader.opcode.value);
            m_server.kill(this);
            return;
        }
        start();
        return;
    }

    m_readBuffer.resize(m_readHeader.length.value);
    auto buffer = asio::buffer(m_readBuffer, m_readHeader.length.value);
    auto fn = std::bind(&RemotePlayer::handleReadBody, this,
                        std::placeholders::_1);
    asio::async_read(m_socket, buffer, fn);
}

void RemotePlayer::handleReadBody(asio::error_code const &error) {
    if (error) {
        displayError(error);
        return;
    }

    if (m_readHeader.length.value > m_readBuffer.size()) {
        m_log.logDebug("[%s] Invalid 0x%04x packet of %d bytes (expected %d bytes)",
                       getRemoteAddress().c_str(),
                       m_readHeader.opcode.value, m_readBuffer.size(),
                       m_readHeader.length.value);
        m_server.kill(this);
        return;
    }

    AbstractPacket *packet = makePacket(m_readHeader.opcode);
    try {
        std::string str(m_readBuffer.begin(), m_readBuffer.end());
        packet->unserialize(str.c_str(), str.size());

        ServerRouter::HandlerType fn;
        fn = m_server.getPacketHandler(m_readHeader.opcode);
        fn(m_server, *this, *packet);
    }
    catch (UnserializeError) {
        m_log.logDebug("[%s] Invalid 0x%04x packet of %d bytes",
                       getRemoteAddress().c_str(),
                       m_readHeader.opcode.value, m_readBuffer.size());
        m_server.kill(this);
        return;
    }

    start();
}

void RemotePlayer::handleWrite(asio::error_code const &error) {
    if (error) {
        displayError(error);
        return;
    }
    m_writeQueue.pop();
    if (!m_writeQueue.empty()) {
        writePacket();
    }
}

asio::ip::tcp::socket &RemotePlayer::getSocket() {
    return m_socket;
}

std::string RemotePlayer::getRemoteAddress() const {
    std::string ra;
    ra += m_socket.remote_endpoint().address().to_string();
    ra += ":";
    ra += std::to_string(m_socket.remote_endpoint().port());
    return ra;
}

void RemotePlayer::displayError(const asio::error_code &error) {
    if (m_socket.is_open()) {
        std::string address;
        try {
            address = getRemoteAddress();
        }
        catch (asio::system_error &e) {
            // remote endpoint is dead but socket is still open
            m_socket.cancel();
            m_socket.close();
            displayError(error);
        }

        std::string message = error.message();
        int value = error.value();
        m_log.logInfo("[%s] Error %d: %s",
                      address.c_str(), value, message.c_str());
    }
    else {
        std::string message = error.message();
        int value = error.value();
        m_log.logInfo("[closed socket] Error %d: %s",
                      value, message.c_str());
    }
}


ServerRouter::ServerRouter()
    : m_acceptor(m_ioService)
    , m_log(coreLog)
{
    ConfigManager &cmi = ConfigManager::GetInstance();

    cmi["core.server.address"].define("0.0.0.0", "Listen address");
    cmi["core.server.port"].define(11299, "Listen port");
    cmi["core.server.ipv6"].define(true, "Use IPv6");
    cmi["core.server.threads"].define(2, "Threads number");

    for(uint16_t i=0; i<0xffff; i++) {
        m_handlers[i] = &nullPacketHandler;
    }
}

void ServerRouter::close() {
    m_acceptor.cancel();
    m_clients.clear();
    m_acceptor.close();

    auto it = m_threads.begin();
    for (; it != m_threads.end(); it++) {
        (*it)->join();
    }



    m_log.logInfo("Closed server");
}

void ServerRouter::listen() {
    /*if (m_acceptor.is_open()) {
        close();
    }*/

    /*
    std::string address = getConfigValue("core.server.address");
    bool useIPv6 = getConfigValue("core.server.ipv6");
    */

    uint16_t port = getConfigValue("core.server.port");
    uint16_t threads = getConfigValue("core.server.threads");

    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 11299);
    //asio::ip::address ipaddress;
    //ipaddress.from_string(address);
    //endpoint.address(ipaddress);
    //endpoint.port(port);

    m_acceptor.open(asio::ip::tcp::v4());
//    m_acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
    m_acceptor.bind(endpoint);
    m_acceptor.listen();

    m_log.logInfo("Listening on port %d with %d threads", port, threads);

    accept();

    // Starting threads
    for (uint16_t i=0; i<threads; i++) {
        auto fn = std::bind(&ServerRouter::threadMain, this);
        std::shared_ptr<asio::thread> ptr(new asio::thread(fn));
        m_threads.push_back(ptr);
    }
}

void ServerRouter::threadMain() {
    m_log.logDebug("thread: running.");
    m_ioService.run();
    m_log.logDebug("thread: done.");
}

void ServerRouter::accept() {
    RemotePlayer *newClient(new RemotePlayer(m_ioService, *this));
    m_clients.push_back(newClient);

    auto fn = std::bind(&ServerRouter::handleAccept, this,
                        std::placeholders::_1,
                        newClient);
    m_acceptor.async_accept(newClient->getSocket(), fn);
}

void ServerRouter::send(AbstractPacket const &packet, RemotePlayer *ptr) {
    ptr->send(packet);
}

void ServerRouter::sendByUsername(AbstractPacket const &packet, std::string const &username) {
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        if ((*it)->getUsername() == username) {
            (*it)->send(packet);
            break;
        }
    }
}

void ServerRouter::sendByAddress(AbstractPacket const &packet, std::string const &address) {
    // TODO: should check port too
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        if ((*it)->getRemoteAddress() == address) {
            (*it)->send(packet);
            break;
        }
    }
}

void ServerRouter::sendByEntity(AbstractPacket const &packet, uint32_t entityId) {
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        if ((*it)->getEntityId() == entityId) {
            (*it)->send(packet);
            break;
        }
    }
}

void ServerRouter::sendAll(AbstractPacket const &packet) {
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        (*it)->send(packet);
    }
}

void ServerRouter::handleAccept(asio::error_code const &error, RemotePlayer *newClient) {
    if (error) {
        if (error == asio::error::operation_aborted) {
            delete newClient;
            return;
        }
        displayError(error);

        return;
    }
    newClient->start();
    accept();
}

void ServerRouter::kill(RemotePlayer *ptr) {
    delete ptr;
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        if (*it == ptr) {
            m_clients.erase(it);
        }
    }
}

void ServerRouter::setPacketHandler(uint16_t opcode, HandlerType function) {
    if (function == NULL) {
        function = &nullPacketHandler;
    }
    m_handlers[opcode] = function;
}

ServerRouter::HandlerType ServerRouter::getPacketHandler(uint16_t opcode) const {
    return m_handlers[opcode];
}

void ServerRouter::displayError(const asio::error_code &error) {
    std::string message = error.message();
    int value = error.value();
    m_log.logInfo("Error %d: %s", value, message.c_str());
}

void ServerRouter::nullPacketHandler(ServerRouter &server, RemotePlayer &client, AbstractPacket const &packet) {
    std::string address = client.getRemoteAddress();
    server.getLog().logDebug("Unhandled packet from [%s]: opcode=%d", address.c_str(), packet.opcode.value);
}
