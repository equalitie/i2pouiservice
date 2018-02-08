#pragma once

#include <boost/asio.hpp>

#include <generic_oui_acceptor.h>
#include <generic_oui_connector.h>

// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

class GenericService {
public:
  GenericService(const std::string& datadir, boost::asio::io_service&);

  GenericService(const GenericService&) = delete;
  GenericService& operator=(const GenericService&) = delete;

  GenericService(GenericService&&);
  GenericService& operator=(GenericService&&);

  boost::asio::io_service& get_io_service();

  ~GenericService();

protected:
  boost::asio::io_service& _ios;
};

