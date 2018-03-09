#pragma once
#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include <vector>
#include <boost/asio.hpp>

#include <i2poui/connection.h>
#include <generic_oui_service.h>
#include <i2poui/server.h>
#include <i2poui/client.h>

namespace i2poui {

  class Service  {
public:

  Service(const std::string& datadir, boost::asio::io_service&);

  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;

  Service(Service&&);
  Service& operator=(Service&&);

  //access functions
  uint32_t  get_i2p_tunnel_ready_timeout() { return 5*60; /* 5 minutes */ };

  boost::asio::io_service& get_io_service() {   return _ios; };

  std::shared_ptr<Server> build_acceptor(std::string private_key_filename);
  std::shared_ptr<Client> build_connector(const std::string& target_id, std::string private_key_filename);

  ~Service();

protected:
  boost::asio::io_service& _ios;
  std::string _data_dir;

  std::vector<std::shared_ptr<Server>> _servers;
  std::vector<std::shared_ptr<Client>> _clients;

  
};

} // i2poui namespace
