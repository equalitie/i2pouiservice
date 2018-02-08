#pragma once

#include <vector>
#include <boost/asio.hpp>

#include <generic_oui_service.h>
#include <i2poui/acceptor.h>
#include <i2poui/connector.h>

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

  boost::asio::io_service& get_io_service();

  ~Service();

protected:
  boost::asio::io_service& _ios;
  std::string _data_dir;

  std::vector<std::shared_ptr<Acceptor>> _acceptors;
  std::vector<std::shared_ptr<Connector>> _connectors;

  std::shared_ptr<Acceptor> build_acceptor(std::string private_key_filename);
  std::shared_ptr<Connector> build_connector(const std::string& target_id, std::string private_key_filename);
  
};

} // i2poui namespace
