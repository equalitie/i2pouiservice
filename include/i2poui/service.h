#pragma once

#include <boost/asio.hpp>

#include <i2poui/channel.h>
#include <i2poui/acceptor.h>
#include <i2poui/connector.h>

// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

namespace i2poui {

class Service {
public:
  using OnBuildAcceptor  = std::function<void(boost::system::error_code, Acceptor)>;
  using OnBuildConnector = std::function<void(boost::system::error_code, Connector)>;

  Service(const std::string& datadir, boost::asio::io_service&);

  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;

  Service(Service&&);
  Service& operator=(Service&&);

  boost::asio::io_service& get_io_service();

  std::string public_identity() const;

  template<class Token>
  auto build_acceptor(Token&&);

  template<class Token>
  auto build_connector(const std::string& target_id, Token&&);

  //access functions
  uint32_t  get_i2p_tunnel_ready_timeout() { return 5*60; /* 5 minutes */ };

  ~Service();

protected:
  void build_acceptor_cb(OnBuildAcceptor);
  void build_connector_cb(const std::string& target_id, OnBuildConnector);

protected:
  boost::asio::io_service& _ios;
  std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
  std::unique_ptr<i2p::data::PrivateKeys> _private_keys;
};

template<class Token>
auto Service::build_acceptor(Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code, Acceptor)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);
    build_acceptor_cb(std::move(handler));
    return result.get();
}

template<class Token>
auto Service::build_connector(const std::string& target_id, Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code, Connector)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);
    build_connector_cb(target_id, std::move(handler));
    return result.get();
}

} // i2poui namespace
