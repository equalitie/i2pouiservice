#include <i2poui/service.h>
#include <i2poui/client.h>

#include "I2PTunnel.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

Client::Client(const string& target_id, std::string private_key_filename, uint32_t timeout, boost::asio::io_service& ios)
  : _ios(ios), _client_tunnel(ios)
{
  std::shared_ptr<i2p::client::I2PClientTunnel> i2p_tunnel =
    std::make_shared<i2p::client::I2PClientTunnel>("i2p_oui_client",
                                                     target_id, "127.0.0.1", 0, nullptr);

  i2p_tunnel->Start();

  //The port gets validated after service is started 
  _port = i2p_tunnel->GetLocalEndpoint().port();

  // We need to set a timeout in order to trigger the timer for checking the
  // tunnel readyness
  i2p_tunnel->SetConnectTimeout(timeout);

  _client_tunnel._i2p_tunnel = i2p_tunnel;
  
}

/**
   is called by GenericConnector::connect to set a callback when
   when a new connection gets connected

*/
ouinet::GenericConnection Client::connect(boost::asio::yield_context yield)
{

  if (!_is_ready) {
    _client_tunnel.wait_to_get_ready(yield);
    _is_ready = true;
  }
  
  _client_tunnel._connections.push_back(std::make_shared<Connection>(_ios));
  std::shared_ptr<Connection> connection_socket =  _client_tunnel._connections.back();

  connection_socket->_socket.async_connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::loopback(), _port), yield);
           
  return ouinet::GenericConnection(move(*connection_socket));

}
