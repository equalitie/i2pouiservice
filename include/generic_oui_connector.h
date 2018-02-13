#pragma once

#include "generic_oui_channel.h"

template<class ConnectorImplementation, class ConnectionImplementation>
class GenericConnector : public GenericChannel<ConnectorImplementation> {
 protected:
  using OnConnect = std::function<void(const boost::system::error_code&, ConnectionImplementation&)>;
  using OnReadyToConnect = std::function<void(const boost::system::error_code&)>;

public:
    /**
       is colled to make a connection to a acceptor target

       @param Token&& the function to be called back when the acceptor is connector a connection.
    */
    template<class Token>
    auto connect(Token&&);


    GenericConnector() {};
    GenericConnector(uint16_t port);

};

template<class ConnectorImplementation, class ConnectionImplementation>
inline
GenericConnector<ConnectorImplementation, ConnectionImplementation>::GenericConnector(uint16_t port)
{
}

template<class ConnectorImplementation, class ConnectionImplementation>
template<class Token>
  auto GenericConnector<ConnectorImplementation, ConnectionImplementation>::connect(Token&& token)
{
  namespace asio = boost::asio;
  namespace sys = boost::system;

  using Handler = typename asio::handler_type
      < Token
    , void(boost::system::error_code, ConnectionImplementation&)>::type;

  Handler handler(std::forward<Token>(token));
  asio::async_result<Handler> result(handler);
  static_cast<ConnectorImplementation*>(this)->connect_cb(std::move(handler));
  return result.get();
}

