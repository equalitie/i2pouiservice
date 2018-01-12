#pragma once

#include <memory>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include "I2PTunnel.h"

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

    void connect(std::string target_id, uint32_t timeout, OnConnect connect_handler);

    template< class MutableBufferSequence
            , class ReadHandler>
    void async_read_some(const MutableBufferSequence&, ReadHandler&&);

    template< class ConstBufferSequence
            , class WriteHandler>
    void async_write_some(const ConstBufferSequence&, WriteHandler&&);

protected:
    friend class Service;
    friend class Acceptor;
    boost::asio::io_service& _ios;
    boost::asio::ip::tcp::socket socket_;
    
    std::shared_ptr<i2p::client::I2PService> i2p_oui_tunnel;
};

template< class MutableBufferSequence
        , class ReadHandler>
void Channel::async_read_some( const MutableBufferSequence& bufs
                             , ReadHandler&& h)
{
    socket_.async_read_some(bufs, std::forward<ReadHandler>(h));
}

template< class ConstBufferSequence
        , class WriteHandler>
void Channel::async_write_some( const ConstBufferSequence& bufs
                              , WriteHandler&& h)
{
    socket_.async_write_some(bufs, std::forward<WriteHandler>(h));
}

} // i2poui namespace
