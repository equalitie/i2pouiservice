#include <fstream>
#include <streambuf>

#include <i2poui/service.h>
#include <i2poui/acceptor.h>

#include "Destination.h"
#include "I2PTunnel.h"
#include "api.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

Acceptor::Acceptor(string private_key_filename, uint32_t timeout, boost::asio::io_service& ios)
  :_ios(ios)
{
  using tcp = boost::asio::ip::tcp;
  using std::move;

  //First we either load or generate the private key
  load_private_key(private_key_filename);
  
  // We have to accept to this port so the i2pservertunnel can forward us the
  // connection
  _tcp_acceptor = make_unique<tcp::acceptor>(ios, tcp::endpoint(tcp::v4(), 0));
  uint16_t port = _tcp_acceptor->local_endpoint().port();

  // We need to make a local destination first.
  std::shared_ptr<i2p::client::ClientDestination> local_dst;
  local_dst = i2p::api::CreateLocalDestination(_private_keys, true);

  _i2p_server_tunnel =
    std::make_shared<i2p::client::I2PServerTunnel>("i2p_oui_server",
                                                   "127.0.0.1", port, local_dst);

  _i2p_server_tunnel->Start();

  // I2Pd doesn't implicitly keep io_service bussy, so we need to
  // do it ourselves.
  auto work = std::make_shared<boost::asio::io_service::work>(ios);

  // We need to set a timeout in order to trigger the timer for checking the
  // tunnel readyness
  _i2p_server_tunnel->SetConnectTimeout(timeout);
  
}

void Acceptor::is_ready_cb(OnReadyToAccept handler)
{
  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl
  _i2p_server_tunnel->AddReadyCallback([ handler = move(handler)](const boost::system::error_code& ec) mutable {
                                         handler(ec);
                                       });

}

void Acceptor::accept_cb(OnAccept handler)
{
  
  _connections.push_back(boost::asio::ip::tcp::socket(_ios));
  boost::asio::ip::tcp::socket& connection_socket =  _connections.back();
  _tcp_acceptor->async_accept( connection_socket,
                              [ this,
                                &connection_socket,
                                h = std::move(handler)
                                ] (const boost::system::error_code& ec) mutable {
                                 h(ec, connection_socket);
                              });

}

std::string Acceptor::public_identity() const
{
  return _private_keys.GetPublic()->ToBase64();
}
