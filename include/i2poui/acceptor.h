#pragma once

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

namespace i2poui {

class Acceptor {
    using OnAccept = std::function<void(const boost::system::error_code&)>;

public:
    Acceptor() {}

    template<class Token>
    void accept(Channel&, Token&&);

private:
    friend class Service;

    // Acceptor is built by the i2poui::Service
    Acceptor( std::shared_ptr<i2p::client::I2PServerTunnel>
            , boost::asio::ip::tcp::acceptor);

    void accept_cb(Channel&, OnAccept);

private:
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
