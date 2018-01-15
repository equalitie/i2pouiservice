#include <boost/bind.hpp>

#include "I2PTunnel.h"
#include "I2PService.h"
#include "api.h"

#include <i2poui/channel.h>
#include <i2poui/service.h>
#include <i2poui/connector.h>

using namespace std;
using namespace i2poui;

namespace ip = boost::asio::ip;

Channel::Channel(Service& service)
  : _ios(service.get_io_service()),
    _socket(_ios)
{
}

boost::asio::io_service& Channel::get_io_service()
{
    return _ios;
}

void Channel::connect_cb( Connector& connector
                        , OnConnect handler)
{
    i2p_oui_tunnel = connector._i2p_tunnel;

    _socket.async_connect(ip::tcp::endpoint(ip::address_v4::loopback(), connector._port),
                          [h = std::move(handler)]
                          (const boost::system::error_code& ec) {
                              h(ec);
                          });
}
