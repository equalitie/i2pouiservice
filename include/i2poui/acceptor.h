#pragma once

namespace i2poui {

class Acceptor {
public:
    Acceptor() {}

    template<class Token>
    void accept(Channel&, Token&&);

private:
    friend class Service;

    // Acceptor is built by the i2poui::Service
    Acceptor( std::shared_ptr<i2p::client::I2PServerTunnel>
            , boost::asio::ip::tcp::acceptor);

private:
    std::shared_ptr<i2p::client::I2PServerTunnel> _i2p_server_tunnel;
    std::unique_ptr<boost::asio::ip::tcp::acceptor> _tcp_acceptor;
};

template<class Token>
inline
void Acceptor::accept(Channel& channel, Token&& token)
{
    namespace asio = boost::asio;
    namespace sys = boost::system;

    using Handler = typename asio::handler_type
        < Token
        , void(boost::system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);

    _tcp_acceptor->async_accept(channel._socket,
                            [ ch = &channel
                            , h = std::move(handler)
                            , tunnel = _i2p_server_tunnel
                            ] (const boost::system::error_code& ec) mutable {
                                ch->i2p_oui_tunnel = std::move(tunnel);
                                h(ec);
                            });

    return result.get();
}

} // i2poui namespace
