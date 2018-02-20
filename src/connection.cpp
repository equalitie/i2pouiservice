#include <i2poui/service.h>

using namespace std;
using namespace i2poui;

namespace ip = boost::asio::ip;

Connection::Connection(Service& service)
  : _ios(service.get_io_service()),
    _socket(_ios)
{
}
