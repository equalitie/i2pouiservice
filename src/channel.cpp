#include <boost/bind.hpp>

#include "I2PService.h"
#include "api.h"

#include <i2poui/channel.h>
#include <i2poui/service.h>

using namespace std;
using namespace i2poui;

namespace ip = boost::asio::ip;

Channel::Channel(Service& service)
  : _ios(service.get_io_service()),
    socket_(_ios)
{
}

boost::asio::io_service& Channel::get_io_service()
{
    return _ios;
}

void Channel::connect( std::string target_id
                     , uint32_t connect_timeout
                     , OnConnect connect_handler)
{
    uint16_t port = rand() % 32768 + 32768;

    i2p_oui_tunnel =
        std::make_unique<i2p::client::I2PClientTunnel>("i2p_oui_client",
                target_id, "127.0.0.1", port, nullptr);

    i2p_oui_tunnel->Start();

    // Wait till we find a route to the service and tunnel is ready then try to
    // acutally connect and then call the handle
    i2p_oui_tunnel->AddReadyCallback([ this
                                     , h = std::move(connect_handler)
                                     , port
                                     ](const boost::system::error_code& ec) {
            if (ec) {
                // NOTE: Executing `h` through post here because I don't know
                // whether AddReadyCallback guarantees not to execute it's
                // handler right a way.
                _ios.post([ec, h = std::move(h)] { h(ec); });
                return;
            }

            bool is_ready = i2p_oui_tunnel->GetLocalDestination()->IsReady();
            assert(is_ready && "TODO: Can it not be ready given (!ec)?");

            socket_.async_connect(ip::tcp::endpoint(ip::address_v4::loopback(), port),
                                  [h = std::move(h)]
                                  (const boost::system::error_code& ec) {
                                      if (ec) {
                                          std::cout << "Error: " << ec.message() << "\n";
                                      }

                                      h(ec);
                                  });
        });

    // We need to set a timeout in order to trigger the timer for checking the
    // tunnel readyness
    i2p_oui_tunnel->SetConnectTimeout(connect_timeout);
}
