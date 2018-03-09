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
class Connector;

class Connection {
public:
  Connection(Service& service); //Not sure we need this
  Connection(boost::asio::io_service& ios)
  : _ios(ios),
    _socket(_ios)
    {
    }

  Connection(const Connection&) = delete;
  Connection& operator=(const Connection&) = delete;

  Connection(Connection&&) = default;
  Connection& operator=(Connection&&) = default;

  boost::asio::io_service& get_io_service() {return _ios;}
  
  template< class MutableBufferSequence
            , class ReadHandler>
    void async_read_some(const MutableBufferSequence&, ReadHandler&&);

  template< class ConstBufferSequence
            , class WriteHandler>
    void async_write_some(const ConstBufferSequence&, WriteHandler&&);

  //TODO::make a realy close, call the server and inform that they don't need
  //this stream
  void close() {
    return;
  }
protected:
  friend class Service;
  friend class Server;
  friend class Client;

  boost::asio::io_service& _ios;
  boost::asio::ip::tcp::socket _socket;
      
};

template< class MutableBufferSequence
        , class ReadHandler>
void Connection::async_read_some( const MutableBufferSequence& bufs
                             , ReadHandler&& h)
{
    _socket.async_read_some(bufs, std::forward<ReadHandler>(h));
}

template< class ConstBufferSequence
        , class WriteHandler>
void Connection::async_write_some( const ConstBufferSequence& bufs
                              , WriteHandler&& h)
{
    _socket.async_write_some(bufs, std::forward<WriteHandler>(h));
}

} // i2poui namespace
