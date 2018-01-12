#include <fstream>
#include <streambuf>
#include <i2poui/service.h>
#include <i2poui/acceptor.h>

//i2p stuff
#include "Log.h"
#include "api.h"

using namespace std;
using namespace i2poui;

static string load_private_key()
{
    string datadir = i2p::fs::GetDataDir();
    string key_file_name = datadir + "/private_key";

    ifstream in_file(key_file_name);

    if (in_file.is_open()) {
        return string( istreambuf_iterator<char>(in_file)
                     , istreambuf_iterator<char>());
    }

    // File doesn't exist
    i2p::data::SigningKeyType sig_type = i2p::data::SIGNING_KEY_TYPE_ECDSA_SHA256_P256;
    i2p::data::PrivateKeys keys = i2p::data::PrivateKeys::CreateRandomKeys(sig_type);
    string keys_str = keys.ToBase64();

    ofstream out_file(key_file_name);
    out_file << keys_str;

    return keys_str;
}

Service::Service(const string& datadir, boost::asio::io_service& ios)
  : _ios(ios)
{
  //here we are going to read the config file and
  //set options based on those values for now we just
  //set it up by some default values;

  //start i2p logger
  i2p::log::Logger().Start();

  LogPrint(eLogInfo, "Starting i2p tunnels");

  string datadir_arg = "--datadir=" + datadir;

  std::vector<const char*> argv({"i2pouiservice", datadir_arg.data()});

  i2p::api::InitI2P(argv.size(), (char**) argv.data(), argv[0]);
  i2p::api::StartI2P();

  _private_keys.FromBase64(load_private_key());
}

boost::asio::io_service& Service::get_io_service()
{
  return _ios;
}

std::string Service::public_identity() const
{
  return _private_keys.GetPublic()->ToBase64();
}

void Service::build_acceptor(OnBuildAcceptor handler)
{
  using tcp = boost::asio::ip::tcp;
  using std::move;

  // We have to accept to this port so the i2pservertunnel can forward us the
  // connection
  auto tcp_acceptor = make_shared<tcp::acceptor>(_ios, tcp::endpoint(tcp::v4(), 0));
  uint16_t port = tcp_acceptor->local_endpoint().port();

  // We need to make a local destination first.
  std::shared_ptr<i2p::client::ClientDestination> local_dst;
  local_dst = i2p::api::CreateLocalDestination(_private_keys, true);

  auto i2p_tunnel =
      std::make_shared<i2p::client::I2PServerTunnel>("i2p_oui_server",
              "127.0.0.1", port, local_dst);

  i2p_tunnel->Start();

  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl
  i2p_tunnel->AddReadyCallback([ handler = move(handler)
                               , tcp_acceptor
                               , i2p_tunnel
                               ](const boost::system::error_code& ec) mutable {
          Acceptor acceptor(i2p_tunnel, move(*tcp_acceptor));
          handler(ec, move(acceptor));
      });

  // We need to set a timeout in order to trigger the timer for checking the
  // tunnel readyness
  i2p_tunnel->SetConnectTimeout(get_i2p_tunnel_ready_timeout());
}
