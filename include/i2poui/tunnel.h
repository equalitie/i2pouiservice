#pragma once
#include <boost/asio/spawn.hpp>
#include "I2PService.h"

#include "ouiservice.h"
#include "blocker.h"

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

namespace i2poui {

  class Tunnel  {
public:
  boost::asio::io_service& get_io_service() {  return _ios; }

  /**
       is called by GenericConnector::is_ready to set a callback when
       the acceptor is ready.

  */
  void wait_to_get_ready(boost::asio::yield_context yield) {
    ouinet::Blocker blocker(_ios);

    // Wait till we find a route to the service and tunnel is ready then try to
    // acutally connect and then unblock
    _i2p_tunnel->AddReadyCallback([block = std::make_shared<ouinet::Blocker::Block>(blocker.make_block())](const boost::system::error_code& ec) {
      });

    // This _returns_ once the `block` thing create above gets destroyed
    // i.e. when the handler finishes.
    blocker.wait(yield);

  }

 protected:
  friend class Client;
  friend class Server;

  // Connector is built by i2poui::Service
  Tunnel(boost::asio::io_service& ios)
    : _ios(ios)
    {
      // I2Pd doesn't implicitly keep io_service bussy, so we need to
      // do it ourselves.
      _waiting_work = std::make_shared<boost::asio::io_service::work>(_ios);
    };

  boost::asio::io_service& _ios;

  //the tunnel will use this mock work to prevent asio service
  //from exiting while channel is waiting for accepting or connecting
  std::shared_ptr<boost::asio::io_service::work> _waiting_work;

  std::shared_ptr<i2p::client::I2PService> _i2p_tunnel;
  std::vector<std::shared_ptr<Connection>> _connections;

};

} // i2poui namespace
