#pragma once

#include <boost/asio.hpp>

#include <i2poui/channel.h>
#include <i2poui/acceptor.h>
#include <i2poui/connector.h>

// Forward declarations of i2p classes
namespace i2p { namespace data {
    class PrivateKeys;
}} // i2p::client namespace

class GenericService {
public:
  Service(const std::string& datadir, boost::asio::io_service&);

  Service(const Service&) = delete;
  Service& operator=(const Service&) = delete;

  Service(Service&&);
  Service& operator=(Service&&);

  boost::asio::io_service& get_io_service();

  shared_ptr<GenericAcceptor> build_acceptor(std::string private_key_filename, std::string private_key_filename);
  shared_ptr<GenericConnector> build_connector(const std::string& target_id);

  ~Service();

protected:
  boost::asio::io_service& _ios;
};

