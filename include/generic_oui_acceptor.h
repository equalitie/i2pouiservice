#pragma once

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PServerTunnel;
}} // i2p::client namespace

template<class AcceptorImplementation>
class GenericAcceptor {
protected:
  using OnAccept = std::function<void(const boost::system::error_code&, boost::asio::ip::tcp::socket&)>;
    using OnReadyToAccept = std::function<void(const boost::system::error_code&)>;
  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl
  
public:
    GenericAcceptor() {}

    /**
       optionally it can be queried to make sure that the acceptor  is actully
       ready to receive connection.

       @param Token&& the function to be called back when the acceptor is ready.
    */
    template<class Token>
    void is_ready(Token&&);
    
    template<class Token>
    void accept(Token&&);

protected:
    friend class GenericService;

};

template <class AcceptorImplementation>
template <class Token>
inline
void GenericAcceptor<AcceptorImplementation>::accept(Token&& token)
{
    using Handler = typename boost::asio::handler_type
        < Token
        , void(boost::system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    boost::asio::async_result<Handler> result(handler);
    static_cast<AcceptorImplementation*>(this)->accept_cb(std::move(handler));
    return result.get();
}

template<class AcceptorImplementation>
template<class Token>
void GenericAcceptor<AcceptorImplementation>::is_ready(Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);
    static_cast<AcceptorImplementation*>(this)->is_ready_cb(std::move(handler));
    return result.get();
}
