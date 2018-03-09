#include <fstream>
#include <streambuf>

#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include "generic_connection.h"
#include <i2poui/service.h>

#include "Destination.h"
#include "I2PTunnel.h"
#include "api.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

void Server::load_private_key(string key_file_name)
{
  ifstream in_file(key_file_name);
  string keys_str;
  if (in_file.is_open()) {
    keys_str = string( istreambuf_iterator<char>(in_file)
                       , istreambuf_iterator<char>());
	
  } else {
    // File doesn't exist
    i2p::data::SigningKeyType sig_type = i2p::data::SIGNING_KEY_TYPE_ECDSA_SHA256_P256;
    i2p::data::PrivateKeys keys = i2p::data::PrivateKeys::CreateRandomKeys(sig_type);
    keys_str = keys.ToBase64();
	
    ofstream out_file(key_file_name);
    out_file << keys_str;
	
  }
  
  _private_keys.FromBase64(keys_str);
  
}

Server::Server(string private_key_filename, uint32_t timeout, boost::asio::io_service& ios)
  :_ios(ios), _server_tunnel(ios)
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

  shared_ptr<i2p::client::I2PServerTunnel>  i2p_server_tunnel =
    std::make_shared<i2p::client::I2PServerTunnel>("i2p_oui_server",
                                                   "127.0.0.1", port, local_dst);

  i2p_server_tunnel->Start();

  // We need to set a timeout in order to trigger the timer for checking the
  // tunnel readyness
  i2p_server_tunnel->SetConnectTimeout(timeout);

  _server_tunnel._i2p_tunnel = i2p_server_tunnel;
  
}

ouinet::GenericConnection Server::accept(boost::asio::yield_context yield)
{
  _server_tunnel._connections.push_back(std::make_shared<Connection>(_ios));
  std::shared_ptr<Connection> i2poui_connection =  _server_tunnel._connections.back();

  _tcp_acceptor->async_accept(i2poui_connection->_socket,
                               yield);

  return ouinet::GenericConnection(move(*i2poui_connection));

}

std::string Server::public_identity() const
{
  return _private_keys.GetPublic()->ToBase64();
}
