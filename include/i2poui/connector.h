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

} // i2poui namespace
