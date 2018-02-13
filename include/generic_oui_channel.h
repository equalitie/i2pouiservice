#pragma once

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PService;
}} // i2p::client namespace

template<class ChannelImplementation>
class GenericChannel {
protected:
  using OnReadyToAccept = std::function<void(const boost::system::error_code&)>;
  // Wait till we find a route to the service and tunnel is ready then try to
  // acutally connect and then call the handl

public:
    /**
       optionally it can be queried to make sure that the channel is actully
       ready to receive connection.

       @param Token&& the function to be called back when the channel is ready.
    */
    template<class Token>
    void is_ready(Token&&);

 };

template<class ChannelImplementation>
template<class Token>
void GenericChannel<ChannelImplementation>::is_ready(Token&& token)
{
    using namespace boost;

    using Handler = typename asio::handler_type
            <Token, void(system::error_code)>::type;

    Handler handler(std::forward<Token>(token));
    asio::async_result<Handler> result(handler);
    static_cast<ChannelImplementation*>(this)->is_ready_cb(std::move(handler));
    return result.get();
    
}
