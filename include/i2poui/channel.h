#pragma once

#include <memory>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

// Forward declarations of i2p classes
namespace i2p { namespace client {
    class I2PService;
}} // i2p::client namespace

namespace i2poui {

class Service;

class Channel {
public:
  using OnConnect = std::function<void(boost::system::error_code)>;

public:
    Channel(Service&);

    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    Channel(Channel&&) = default;
    Channel& operator=(Channel&&) = default;

    boost::asio::io_service& get_io_service();

    template<class Token>
    void connect(std::string target_id, uint32_t timeout, Token&&);

    template< class MutableBufferSequence
            , class ReadHandler>
    void async_read_some(const MutableBufferSequence&, ReadHandler&&);

    template< class ConstBufferSequence
            , class WriteHandler>
    void async_write_some(const ConstBufferSequence&, WriteHandler&&);

protected:
    void connect_cb(std::string target_id, uint32_t timeout, OnConnect connect_handler);

protected:
    friend class Service;
    friend class Acceptor;
    boost::asio::io_service& _ios;
    boost::asio::ip::tcp::socket _socket;
    
    std::shared_ptr<i2p::client::I2PService> i2p_oui_tunnel;
};

template< class MutableBufferSequence
        , class ReadHandler>
void Channel::async_read_some( const MutableBufferSequence& bufs
                             , ReadHandler&& h)
{
    _socket.async_read_some(bufs, std::forward<ReadHandler>(h));
}

template< class ConstBufferSequence
        , class WriteHandler>
void Channel::async_write_some( const ConstBufferSequence& bufs
                              , WriteHandler&& h)
{
    _socket.async_write_some(bufs, std::forward<WriteHandler>(h));
}

template<class Token>
void Channel::connect(std::string target_id, uint32_t timeout, Token&& token)
{
  namespace asio = boost::asio;
  namespace sys = boost::system;

  using Handler = typename asio::handler_type
      < Token
      , void(boost::system::error_code)>::type;

  Handler handler(std::forward<Token>(token));
  asio::async_result<Handler> result(handler);
  connect_cb(std::move(target_id), timeout, std::move(handler));
  return result.get();
}

} // i2poui namespace
