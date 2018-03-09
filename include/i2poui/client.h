#pragma once
#include "ouiservice.h"
#include "i2poui/tunnel.h"

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

namespace i2poui {

  class Client : public ouinet::OuiServiceImplementationClient  {
public:
  boost::asio::io_service& get_io_service() {  return _ios; }

  /**
       is called by GenericConnector::connect to set a callback when
       when a new connection gets connected

  */
  ouinet::GenericConnection connect(boost::asio::yield_context yield);

  void cancel_connect(boost::asio::yield_context yield) {};

 protected:
  friend class Service;
  // Connector is built by i2poui::Service
  Client(const std::string& target_id, std::string private_key_filename, uint32_t timeout, boost::asio::io_service& ios);

  boost::asio::io_service& _ios;
  Tunnel _client_tunnel;
  uint16_t _port;

  //false by default, client waits for client_tunnel and then when it is ready, it sets it to true
  bool _is_ready = false;

};

} // i2poui namespace
