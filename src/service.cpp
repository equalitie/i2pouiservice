#include <vector>

#include <i2poui/service.h>
#include <i2poui/acceptor.h>

//i2p stuff
#include "I2PTunnel.h"
#include "Log.h"
#include "api.h"

using namespace std;
using namespace i2poui;

Service::Service(const string& datadir, boost::asio::io_service& ios)
  : _data_dir(datadir)
  , _ios(ios)
    //  , _private_keys(std::make_unique<i2p::data::PrivateKeys>())
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

}

Service::Service(Service&& other)
  : _data_dir(move(other._data_dir))
  ,_ios(other._ios)
  , _acceptors(move(other._acceptors))
{}

Service& Service::operator=(Service&& other)
{
    assert(&_ios == &other._ios);
    _acceptors = move(other._acceptors);
    _data_dir = move(other._data_dir);
    return *this;
}

boost::asio::io_service& Service::get_io_service()
{
  return _ios;
}

Service::~Service() {}

void Service::build_acceptor(std::string private_key_filename)
{
    using namespace boost;

    _acceptors.push_back(std::make_shared<Acceptor>(_data_dir + "/" + private_key_filename, get_i2p_tunnel_ready_timeout(), _ios, std::move(handler)));

    return _acceptors.back();
}

GenericConnector Service::build_connector(std::string private_key_filename)
{
    using namespace boost;

    _connectors.push_back(std::make_shared<Connectors>(_data_dir + "/" + private_key_filename, get_i2p_tunnel_ready_timeout(), _ios, std::move(handler)));

    return _connectors.back();
}
