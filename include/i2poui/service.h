#pragma once

#include <vector>
#include <boost/asio.hpp>

#include <generic_oui_service.h>
#include <i2poui/channel.h>
#include <i2poui/acceptor.h>
#include <i2poui/connector.h>

namespace i2poui {

  class Service public : GenericService {
public:

  Service(const std::string& datadir, boost::asio::io_service&);

  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;

  Service(Service&&);
  Service& operator=(Service&&);

  //access functions
  uint32_t  get_i2p_tunnel_ready_timeout() { return 5*60; /* 5 minutes */ };

  ~Service();

protected:
  shared_ptr<GenericAcceptor> build_acceptor(std::string private_key_filename);
  shared_ptr<GenericConnector> build_connector(const std::string& target_id, std::string private_key_filename);

  std::string _data_dir;
  
};

} // i2poui namespace
