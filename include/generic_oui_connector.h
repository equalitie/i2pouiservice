#pragma once

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

template<class GenericConnectorImplementation>
class GenericConnector {
 protected:
  using OnConnect = std::function<void(const boost::system::error_code&, i2poui::Connection&)>;
  using OnReadyToConnect = std::function<void(const boost::system::error_code&)>;

public:
      /**
       optionally it can be queried to make sure that the acceptor  is actully
       ready to receive connection.

       @param Token&& the function to be called back when the acceptor is ready.
    */
    template<class Token>
    void is_ready(Token&&);

    /**
       is colled to make a connection to a i2p target

       @param Token&& the function to be called back when the acceptor is connector a connection.
    */
    template<class Token>
    auto connect(Token&&);


    GenericConnector() {};
    GenericConnector(uint16_t port, std::shared_ptr<i2p::client::I2PClientTunnel>);

};

template<class ConnectorImplementation>
inline
GenericConnector<ConnectorImplementation>::GenericConnector(uint16_t port,
        std::shared_ptr<i2p::client::I2PClientTunnel> i2p_tunnel)
{
}

//TODO: This should be the same as acceptor::is_ready
//This is channel is ready basically
template<class ConnectorImplementation>
template<class Token>
void GenericConnector<ConnectorImplementation>::is_ready(Token&& token)
{
  using namespace boost;

  using Handler = typename asio::handler_type
    <Token, void(system::error_code)>::type;

  Handler handler(std::forward<Token>(token));
  asio::async_result<Handler> result(handler);
  static_cast<ConnectorImplementation*>(this)->is_ready_cb(std::move(handler));
  result.get();

}


template<class ConnectorImplementation>
template<class Token>
auto GenericConnector<ConnectorImplementation>::connect(Token&& token)
{
  namespace asio = boost::asio;
  namespace sys = boost::system;

  using Handler = typename asio::handler_type
      < Token
    , void(boost::system::error_code, i2poui::Connection)>::type;

  Handler handler(std::forward<Token>(token));
  asio::async_result<Handler> result(handler);
  static_cast<ConnectorImplementation*>(this)->connect_cb(std::move(handler));
  return result.get();
}

