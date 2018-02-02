#pragma once

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

namespace i2poui {

class Connector {
public:
    Connector() {};
    Connector(uint16_t port, std::shared_ptr<i2p::client::I2PClientTunnel>);

private:
    friend class Channel;

    uint16_t _port;
    std::shared_ptr<i2p::client::I2PClientTunnel> _i2p_tunnel;
};

inline
Connector::Connector(uint16_t port,
        std::shared_ptr<i2p::client::I2PClientTunnel> i2p_tunnel)
    : _port(port)
    , _i2p_tunnel(std::move(i2p_tunnel))
{
}

 template<class Token>
auto Service::build_connector(const std::string& target_id, Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code, Connector)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);
    static_cast<ServiceImplementation*>(this)->build_connector_cb(target_id, std::move(handler));
    return result.get();
}

} // i2poui namespace
