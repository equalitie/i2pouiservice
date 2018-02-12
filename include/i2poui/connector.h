#pragma once
#include "generic_oui_connector.h"

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

namespace i2poui {

class Connector : public  GenericConnector<Connector> {
public:
  /**
       is called by GenericConnector::is_ready to set a callback when
       the acceptor is ready.

  */
  void is_ready_cb(OnReadyToConnect handler);

  /**
       is called by GenericConnector::connect to set a callback when
       when a new connection gets connected

  */
  void connect_cb(OnConnect handler);

 protected:
  friend class Service;
  // Connector is built by i2poui::Service
  Connector(const std::string& target_id, std::string private_key_filename, uint32_t timeout, boost::asio::io_service& ios);

  boost::asio::io_service& _ios;
  uint16_t _port;
  std::shared_ptr<i2p::client::I2PClientTunnel> _i2p_tunnel;
  std::vector<boost::asio::ip::tcp::socket> _connections;

};

} // i2poui namespace
