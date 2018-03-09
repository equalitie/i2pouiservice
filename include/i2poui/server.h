#pragma once
#include <boost/asio/spawn.hpp>
#include "Identity.h"
#include "ouiservice.h"
#include "generic_connection.h"

#include <i2poui/tunnel.h>

// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

namespace i2poui {

  class Server : public ouinet::OuiServiceImplementationServer  {

public:

    /**
       blocks the coroutine till a new connection arrives and gets accepted

    */
    ouinet::GenericConnection accept(boost::asio::yield_context yield);

    void start_listen(boost::asio::yield_context yield) {};
	void stop_listen(boost::asio::yield_context yield){};
	
	void cancel_accept(){};
	
	bool is_accepting(){return _ready_state;};

    std::string public_identity() const;

protected:
    friend class Service;
    // Acceptor is built by the i2poui::Service
    Server(std::string private_key_filename, uint32_t timeout, boost::asio::io_service& ios);

    void load_private_key(std::string key_file_name);

    boost::asio::io_service& _ios;
    
    i2p::data::PrivateKeys _private_keys;
    Tunnel _server_tunnel;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _tcp_acceptor;

    bool _ready_state = false;
    
};

} // i2poui namespace
