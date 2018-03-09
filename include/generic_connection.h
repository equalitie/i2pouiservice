#pragma once

#include "namespaces.h"

#include <boost/system/error_code.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/async_result.hpp>
#include <boost/asio/spawn.hpp>
#include <functional>
#include <vector>
#include <iostream>

namespace ouinet {

class GenericConnection {
private:
    template<class Token, class Ret>
    using Handler = typename asio::handler_type< Token
                                               , void(sys::error_code, Ret)
                                               >::type;

    template<class Token, class Ret>
    using Result = typename asio::async_result<Handler<Token, Ret>>;

    using OnRead  = std::function<void(sys::error_code, size_t)>;
    using OnWrite = std::function<void(sys::error_code, size_t)>;

    using ReadBuffers  = std::vector<asio::mutable_buffer>;
    using WriteBuffers = std::vector<asio::const_buffer>;

    struct Base {
        virtual asio::io_service& get_io_service() = 0;

        virtual void read_impl (OnRead&&)  = 0;
        virtual void write_impl(OnWrite&&) = 0;

        virtual void close() = 0;

        virtual ~Base() {}

        ReadBuffers  read_buffers;
        WriteBuffers write_buffers;
    };

    template<class Impl>
    struct Wrapper : public Base {
        Wrapper(Impl&& impl)
            : _impl(std::move(impl))
        {}

        virtual asio::io_service& get_io_service() override
        {
            return _impl.get_io_service();
        }

        void read_impl(OnRead&& on_read) override
        {
            _impl.async_read_some(read_buffers, std::move(on_read));
        }

        void write_impl(OnWrite&& on_write) override
        {
            _impl.async_write_some(write_buffers, std::move(on_write));
        }

        void close() override
        {
            _impl.close();
        }

    private:
        Impl _impl;
    };

public:
    GenericConnection() {}

    template<class AsyncRWStream>
    GenericConnection(AsyncRWStream&& impl)
        : _impl(new Wrapper<AsyncRWStream>(std::forward<AsyncRWStream>(impl)))
    {}

    asio::io_service& get_io_service()
    {
        return _impl->get_io_service();
    }

    template< class MutableBufferSequence
            , class Token>
    typename Result<Token, size_t>::type
    async_read_some(const MutableBufferSequence& bs, Token&& token)
    {
        using namespace std;

        Handler<Token, size_t> handler(forward<Token>(token));
        Result<Token, size_t> result(handler);

        _impl->read_buffers.resize(distance(bs.begin(), bs.end()));
        copy(bs.begin(), bs.end(), _impl->read_buffers.begin());

        _impl->read_impl(move(handler));

        return result.get();
    }

    template< class ConstBufferSequence
            , class Token>
    typename Result<Token, size_t>::type
    async_write_some(const ConstBufferSequence& bs, Token&& token)
    {
        using namespace std;

        Handler<Token, size_t> handler(forward<Token>(token));
        Result<Token, size_t> result(handler);

        _impl->write_buffers.resize(distance(bs.begin(), bs.end()));
        copy(bs.begin(), bs.end(), _impl->write_buffers.begin());

        _impl->write_impl(move(handler));

        return result.get();
    }

    void close()
    {
        _impl->close();
    }

private:
    std::unique_ptr<Base> _impl;
};

} // ouinet namespace
