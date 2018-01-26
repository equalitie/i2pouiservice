#pragma once

// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

namespace i2poui {

class Acceptor {
    using OnAccept = std::function<void(const boost::system::error_code&)>;

public:
    using OnBuildAcceptor  = std::function<void(boost::system::error_code, Acceptor)>;

    Acceptor() {}

    template<class Token>
    void accept(Channel&, Token&&);

    std::string public_identity() const;

 protected:
    friend class Service;
    void load_private_key(std::string key_file_name);
    
    // Acceptor is built by the i2poui::Service
    Acceptor(std::string private_key_filename, uint32_t timeout, boost::asio::io_service& _ios, OnAccept handler);

    void accept_cb(Channel&, OnAccept);

    std::unique_ptr<i2p::data::PrivateKeys> _private_keys;
    std::shared_ptr<i2p::client::I2PServerTunnel> _i2p_server_tunnel;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _tcp_acceptor;
};

template<class Token>
inline
void Acceptor::accept(Channel& channel, Token&& token)
{
    using Handler = typename boost::asio::handler_type
        < Token
        , void(boost::system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    boost::asio::async_result<Handler> result(handler);
    accept_cb(channel, std::move(handler));
    return result.get();
}

} // i2poui namespace
