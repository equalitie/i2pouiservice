#include <i2poui/service.h>
#include <i2poui/channel.h>
#include <i2poui/acceptor.h>

#include "I2PTunnel.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

Acceptor::Acceptor( std::shared_ptr<i2p::client::I2PServerTunnel> i2p_server_tunnel
                  , tcp::acceptor tcp_acceptor)
    : _i2p_server_tunnel(move(i2p_server_tunnel))
    , _tcp_acceptor(std::make_unique<tcp::acceptor>(move(tcp_acceptor)))
{
}

void Acceptor::accept_cb(Channel& channel, OnAccept handler)
{
    _tcp_acceptor->async_accept(channel._socket,
                            [ ch = &channel
                            , h = std::move(handler)
                            , tunnel = _i2p_server_tunnel
                            ] (const boost::system::error_code& ec) mutable {
                                ch->i2p_oui_tunnel = tunnel;
                                h(ec);
                            });
}
