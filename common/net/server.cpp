#include "common/net/server.h"
#include "common/config.h"
#include "common/logging.h"

#include <functional>

using namespace Ponyca;
using namespace Ponyca::Net;


// TODO: Generate that with opcodes_generator.py
class TestPacket : public AbstractPacket {
public:
    TestPacket()
        : AbstractPacket(0)
    {}
    virtual std::string serialize() const {
        return "";
    }
    virtual uint16_t unserialize(char const * buffer) {
        return 0;
    }
};
AbstractPacket *makePacket(int16_t opcode) {
    switch(opcode) {
    case 0x0000: return new TestPacket();
    default: return NULL;
    }
}



RemoteClient::RemoteClient(asio::io_service &ioService, ServerRouter &server)
    : m_socket(ioService)
    , associatedEntity(0)
    , m_server(server)
    , m_log(m_server.getLog())
{

}

RemoteClient::~RemoteClient() {
    if (m_socket.is_open()) {
        m_socket.cancel();
        m_socket.close();
    }
}

void RemoteClient::start() {
    m_readBuffer.clear();
    m_readBuffer.resize(4);
    auto fn = std::bind(&RemoteClient::handleReadHeader, this,
                        std::placeholders::_1);
    auto buffer = asio::buffer(m_readBuffer, 4);
    asio::async_read(m_socket, buffer, fn);
}

void RemoteClient::send(AbstractPacket const &packet) {
    // TODO
}

void RemoteClient::handleReadHeader(asio::error_code const &error) {
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

    m_readHeader.setBufferEnd(m_readBuffer.data()+4);
    m_readHeader.unserialize(m_readBuffer.data());

    if (m_readHeader.length == 0) {
        // we do not expect a body
        start();
        return;
    }

    m_readBuffer.resize(m_readHeader.length);
    auto buffer = asio::buffer(m_readBuffer, m_readHeader.length);
    auto fn = std::bind(&RemoteClient::handleReadBody, this,
                        std::placeholders::_1);
    asio::async_read(m_socket, buffer, fn);
}

void RemoteClient::handleReadBody(asio::error_code const &error) {
    if (error) {
        displayError(error);
        return;
    }

    if (m_readHeader.length > m_readBuffer.size()) {
        m_log.logDebug("[%s] Invalid 0x%04x packet of %d bytes (expected %d bytes)",
                       getRemoteAddress().c_str(),
                       m_readHeader.opcode, m_readBuffer.size(),
                       m_readHeader.length);
        m_server.kill(this);
        return;
    }

    AbstractPacket *packet = makePacket(m_readHeader.opcode);
    try {
        std::string str(m_readBuffer.begin(), m_readBuffer.end());
        packet->setBufferEnd(str.c_str()+m_readHeader.length);
        packet->unserialize(str.c_str());

        ServerRouter::HandlerType fn;
        fn = m_server.getPacketHandler(m_readHeader.opcode);
        fn(m_server, *this, *packet);
    }
    catch (UnserializeError) {
        m_log.logDebug("[%s] Invalid 0x%04x packet of %d bytes",
                       getRemoteAddress().c_str(),
                       m_readHeader.opcode, m_readBuffer.size());
        m_server.kill(this);
        return;
    }

    start();
}

void RemoteClient::handleWrite(asio::error_code const &error) {
    if (error) {
        displayError(error);
        return;
    }
    // TODO
}

asio::ip::tcp::socket &RemoteClient::getSocket() {
    return m_socket;
}

std::string RemoteClient::getRemoteAddress() const {
    std::string ra;
    ra += m_socket.remote_endpoint().address().to_string();
    ra += ":";
    ra += std::to_string(m_socket.remote_endpoint().port());
    return ra;
}

void RemoteClient::displayError(const asio::error_code &error) {
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
    RemoteClient *newClient(new RemoteClient(m_ioService, *this));
    m_clients.push_back(newClient);

    auto fn = std::bind(&ServerRouter::handleAccept, this,
                        std::placeholders::_1,
                        newClient);
    m_acceptor.async_accept(newClient->getSocket(), fn);
}

void ServerRouter::send(AbstractPacket const &packet, RemoteClient *ptr) {
    ptr->send(packet);
}

void ServerRouter::sendByUsername(AbstractPacket const &packet, std::string const &username) {
    auto it = m_clients.begin();
    for (; it != m_clients.end(); it++) {
        if ((*it)->username == username) {
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
        if ((*it)->associatedEntity == entityId) {
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

void ServerRouter::handleAccept(asio::error_code const &error, RemoteClient *newClient) {
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

void ServerRouter::kill(RemoteClient *ptr) {
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

void ServerRouter::nullPacketHandler(ServerRouter &server, RemoteClient &client, AbstractPacket const &packet) {
    std::string address = client.getRemoteAddress();
    server.getLog().logDebug("Unhandled packet from [%s]: opcode=%d", address.c_str(), packet.opcode);
}
