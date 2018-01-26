#include <fstream>
#include <streambuf>

#include <i2poui/service.h>
#include <i2poui/channel.h>
#include <i2poui/acceptor.h>

#include "I2PTunnel.h"
#include "api.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

void Acceptor::load_private_key(string key_file_name)
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

    _private_keys->FromBase64(keys_str);
    
}

Acceptor::Acceptor(string private_key_filename, uint32_t timeout, boost::asio::io_service& ios, OnAccept handler)
  :_private_keys(std::make_unique<i2p::data::PrivateKeys>())
{
  using tcp = boost::asio::ip::tcp;
  using std::move;

  // We have to accept to this port so the i2pservertunnel can forward us the
  // connection
  _tcp_acceptor = make_unique<tcp::acceptor>(ios, tcp::endpoint(tcp::v4(), 0));
  uint16_t port = _tcp_acceptor->local_endpoint().port();

  // We need to make a local destination first.
  std::shared_ptr<i2p::client::ClientDestination> local_dst;
  local_dst = i2p::api::CreateLocalDestination(*_private_keys, true);

  _i2p_server_tunnel =
    std::make_shared<i2p::client::I2PServerTunnel>("i2p_oui_server",
                                                   "127.0.0.1", port, local_dst);

  _i2p_server_tunnel->Start();

  // I2Pd doesn't implicitly keep io_service bussy, so we need to
  // do it ourselves.
  auto work = std::make_shared<boost::asio::io_service::work>(ios);

  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl
  _i2p_server_tunnel->AddReadyCallback([ handler = move(handler)
                               , work
                               ](const boost::system::error_code& ec) mutable {
          handler(ec);
      });

  // We need to set a timeout in order to trigger the timer for checking the
  // tunnel readyness
  _i2p_server_tunnel->SetConnectTimeout(timeout);
  
}

void Acceptor::accept_cb(Channel& channel, OnAccept handler)
{
    _tcp_acceptor->async_accept(channel._socket,
                            [ ch = &channel
                            , h = std::move(handler)
                            , tunnel = _i2p_server_tunnel
                            ] (const boost::system::error_code& ec) mutable {
                                ch->i2p_oui_tunnel = tunnel;
                                h(ec);
                            });
}

std::string Acceptor::public_identity() const
{
  return _private_keys->GetPublic()->ToBase64();
}
