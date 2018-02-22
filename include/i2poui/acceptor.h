#pragma once
#include "generic_oui_acceptor.h"
#include "connection.h"
#include "Identity.h"
// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

namespace i2poui {

class Acceptor : public GenericAcceptor<Acceptor, Connection> {

public:

  //Acceptor() {}

    /**
       is called by GenericAcceptor::is_ready to set a callback when
       the acceptor is ready.

    */
    void is_ready_cb(OnReadyToAccept handler);

    /**
       is called by GenericAcceptor::accept to set a callback when
       when a new connection gets accepted

    */
    void accept_cb(OnAccept handler);

    std::string public_identity() const;

protected:
    friend class Service;
    // Acceptor is built by the i2poui::Service
    Acceptor(std::string private_key_filename, uint32_t timeout, boost::asio::io_service& ios);

    void load_private_key(std::string key_file_name);

    boost::asio::io_service& _ios;
    
    i2p::data::PrivateKeys _private_keys;
    std::shared_ptr<i2p::client::I2PServerTunnel> _i2p_server_tunnel;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _tcp_acceptor;

    std::vector<std::shared_ptr<Connection>> _connections;
    
};

} // i2poui namespace
