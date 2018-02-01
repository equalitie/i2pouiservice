#include <boost/bind.hpp>

#include "I2PTunnel.h"
#include "I2PService.h"
#include "api.h"

#include <i2poui/channel.h>
#include <i2poui/service.h>
#include <i2poui/connector.h>

using namespace std;
using namespace i2poui;

namespace ip = boost::asio::ip;

void Channel::load_private_key(string key_file_name)
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

Channel::Channel(Service& service)
  : _ios(service.get_io_service()),
    _socket(_ios)
{
}

boost::asio::io_service& Channel::get_io_service()
{
    return _ios;
}

void Channel::connect_cb( Connector& connector
                        , OnConnect handler)
{
    i2p_oui_tunnel = connector._i2p_tunnel;

    _socket.async_connect(ip::tcp::endpoint(ip::address_v4::loopback(), connector._port),
                          [h = std::move(handler)]
                          (const boost::system::error_code& ec) {
                              h(ec);
                          });
}
