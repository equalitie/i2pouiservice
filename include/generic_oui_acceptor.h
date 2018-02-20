#pragma once

#include "generic_oui_channel.h"

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

template<class AcceptorImplementation, class ConnectionImplementation>
class GenericAcceptor : GenericChannel<AcceptorImplementation>{
protected:
  using OnAccept = std::function<void(const boost::system::error_code&, ConnectionImplementation*)>;
    using OnReadyToAccept = std::function<void(const boost::system::error_code&)>;
  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl
  
public:
    GenericAcceptor() {}
    
    template<class Token>
    auto accept(Token&&);

protected:
    friend class GenericService;

};

template <class AcceptorImplementation, class ConnectionImplementation>
template <class Token>
inline
  auto GenericAcceptor<AcceptorImplementation, ConnectionImplementation>::accept(Token&& token)
{
    using Handler = typename boost::asio::handler_type
        < Token
      , void(boost::system::error_code , ConnectionImplementation*)>::type;

    Handler handler(std::forward<Token>(token));
    boost::asio::async_result<Handler> result(handler);
    static_cast<AcceptorImplementation*>(this)->accept_cb(std::move(handler));
    return result.get();
}
