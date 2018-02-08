#pragma once

namespace i2p { namespace client {
    class I2PClientTunnel;
}}

template<class GenericConnectorImplementation>
class GenericConnector {
public:
      using OnReadyToConnect = std::function<void(const boost::system::error_code&)>;

    GenericConnector() {};
    GenericConnector(uint16_t port, std::shared_ptr<i2p::client::I2PClientTunnel>);

};

template<class ConnectorImplementation>
inline
GenericConnector<ConnectorImplementation>::GenericConnector(uint16_t port,
        std::shared_ptr<i2p::client::I2PClientTunnel> i2p_tunnel)
{
}

/* template<class Token> */
/* auto Connector::build_connector(const std::string& target_id, Token&& token) */
/* { */
/*     using namespace boost; */

/*     using Handler = typename asio::handler_type */
/*             <Token, void(system::error_code, Connector)>::type; */

/*     Handler handler(std::forward<Token>(token)); */
/*     asio::async_result<Handler> result(handler); */
/*     static_cast<ServiceImplementation*>(this)->build_connector_cb(target_id, std::move(handler)); */
/*     return result.get(); */
/* } */

