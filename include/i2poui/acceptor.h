#pragma once
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

class Acceptor public: Channel {
    using OnAccept = std::function<void(const boost::system::error_code&)>;

public:
    using OnBuildAcceptor  = std::function<void(boost::system::error_code, Acceptor)>;

    Acceptor()
     {}

    template<class Token>
    void accept(Channel&, Token&&);

    std::string public_identity() const;
    
    // Acceptor is built by the i2poui::Service
    Acceptor(std::string private_key_filename, uint32_t timeout, boost::asio::io_service& _ios, OnAccept handler);

 protected:
    friend class Service;
    void load_private_key(std::string key_file_name);

    i2p::data::PrivateKeys _private_keys;
    std::shared_ptr<i2p::client::I2PServerTunnel> _i2p_server_tunnel;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _tcp_acceptor;
};

template<class Token>
inline
void Acceptor::accept(Token&& token)
{
    using Handler = typename boost::asio::handler_type
        < Token
        , void(boost::system::error_code)>::type;

    Handler handler(std::forward<Token>(token));

    _connections.push_back(std::make_shared<boost::asio::ip::tcp::socket>());
    connection_socket = _connections.back()
    _tcp_acceptor->async_accept(connection_socket,
                                [ this,
                                  connection_socket,
                                  h = std::move(handler);
                                  ] (const boost::system::error_code& ec) mutable {
                                  h(connection_socket);
                            });

   return connection_socket;

} // i2poui namespace
