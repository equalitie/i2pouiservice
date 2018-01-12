#pragma once

namespace i2poui {

class Acceptor {
public:
    using OnAccept = std::function<void(const boost::system::error_code&)>;

public:
    template<class Token>
    void accept(Channel&, Token&&);

private:
    friend class Service;

    // Acceptor is built by the i2poui::Service
    Acceptor( std::shared_ptr<i2p::client::I2PServerTunnel>
            , boost::asio::ip::tcp::acceptor);

private:
    std::shared_ptr<i2p::client::I2PServerTunnel> _i2p_server_tunnel;
    boost::asio::ip::tcp::acceptor _tcp_acceptor;
};

template<class Token>
void Acceptor::accept(Channel& channel, Token&& token)
{
  namespace asio = boost::asio;
  namespace sys = boost::system;

  using Handler = typename asio::handler_type
      < Token
      , void(const sys::error_code&)>::type;

  Handler handler(std::forward<Token>(token));
  asio::async_result<Handler> result(handler);

  _tcp_acceptor.async_accept(channel.socket_,
                          [ ch = &channel
                          , h = std::move(handler)
                          , tunnel = _i2p_server_tunnel
                          ] (const boost::system::error_code& ec) {
                              ch->i2p_oui_tunnel = std::move(tunnel);
                              h(ec);
                          });

  return result.get();
}

} // i2poui namespace
