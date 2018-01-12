#include <i2poui/service.h>
#include <i2poui/channel.h>
#include <i2poui/acceptor.h>

using namespace std;
using namespace i2poui;

Acceptor::Acceptor( std::shared_ptr<i2p::client::I2PServerTunnel> i2p_server_tunnel
                  , boost::asio::ip::tcp::acceptor tcp_acceptor)
    : _i2p_server_tunnel(move(i2p_server_tunnel))
    , _tcp_acceptor(move(tcp_acceptor))
{
}

