#include <i2poui/service.h>
#include <i2poui/channel.h>
#include <i2poui/acceptor.h>

#include "I2PTunnel.h"

using namespace std;
using namespace i2poui;

using tcp = boost::asio::ip::tcp;

Connector::Connector(const string& target_id, OnBuildConnector handler)
{
    _i2p_tunnel =
          std::make_shared<i2p::client::I2PClientTunnel>("i2p_oui_client",
                target_id, "127.0.0.1", 0, nullptr);

    _i2p_tunnel->Start();

    //The port gets validated after service is started 
    _port = _i2p_tunnel->GetLocalEndpoint().port();

    // I2Pd doesn't implicitly keep io_service bussy, so we need to
    // do it ourselves.
    auto work = std::make_shared<boost::asio::io_service::work>(_ios);

    i2p_tunnel->AddReadyCallback([ &ios = _ios
                                 , h = std::move(handler)
                                 , work
                                 ](const boost::system::error_code& ec) {

            // NOTE: Executing `h` through post here because I don't know
            // whether AddReadyCallback guarantees not to execute it's
            // handler right a way.
            ios.post([ec, h = std::move(h), c = std::move(connector)] {
                    h(ec, std::move(c));
                });

        });

    // We need to set a timeout in order to trigger the timer for checking the
    // tunnel readyness
    _i2p_tunnel->SetConnectTimeout(get_i2p_tunnel_ready_timeout());
}
