#pragma once

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

namespace i2poui {

class Connector {
public:
  using OnBuildConnector = std::function<void(boost::system::error_code, Connector)>;

  Connector(const std::string& target_id, OnBuildConnector handler);

private:
    friend class Channel;

    uint16_t _port;
    std::shared_ptr<i2p::client::I2PClientTunnel> _i2p_tunnel;
};

} // i2poui namespace
