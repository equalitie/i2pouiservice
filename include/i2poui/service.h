#pragma once

#include <vector>
#include <boost/asio.hpp>

#include <i2poui/channel.h>
#include <i2poui/acceptor.h>
#include <i2poui/connector.h>

namespace i2poui {

class Service {
public:

  Service(const std::string& datadir, boost::asio::io_service&);

  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;

  Service(Service&&);
  Service& operator=(Service&&);

  boost::asio::io_service& get_io_service();

  template<class Token>
  auto build_acceptor(std::string private_key_filename, Token&&);

  template<class Token>
  auto build_connector(const std::string& target_id, Token&&);

  //access functions
  uint32_t  get_i2p_tunnel_ready_timeout() { return 5*60; /* 5 minutes */ };

  ~Service();

protected:
  std::string _data_dir;
  boost::asio::io_service& _ios;
  std::vector<std::shared_ptr<Acceptor>> _acceptors;
  
};

template<class Token>
  auto Service::build_acceptor(std::string private_key_filename, Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    _acceptors.push_back(std::make_shared<Acceptor>(_data_dir + "/" + private_key_filename, get_i2p_tunnel_ready_timeout(), _ios, std::move(handler)));
    return _acceptors.back();
}

template<class Token>
auto Service::build_connector(const std::string& target_id, Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    return std::move(Connector(target_id, get_i2p_tunnel_ready_timeout(), _ios, std::move(handler)));
}

} // i2poui namespace
