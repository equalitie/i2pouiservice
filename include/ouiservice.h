#pragma once

#include <memory>
#include <boost/asio/spawn.hpp>

#include "generic_connection.h"

#define BOOST_COROUTINES_NO_DEPRECATION_WARNING
namespace ouinet {

class OuiServiceImplementationServer
{
	public:
	virtual ~OuiServiceImplementationServer() {}
	
	virtual void start_listen(asio::yield_context yield) = 0;
	virtual void stop_listen(asio::yield_context yield) = 0;
	
	virtual GenericConnection accept(asio::yield_context yield) = 0;
	virtual void cancel_accept() = 0;
	
	virtual bool is_accepting() = 0;
};

class OuiServiceServer
{
	public:
	OuiServiceServer(asio::io_service& ios);
	
	void add(std::unique_ptr<OuiServiceImplementationServer> implementation);
	
	void start_listen(asio::yield_context yield);
	void stop_listen(asio::yield_context yield);
	
	GenericConnection accept(asio::yield_context yield);
	void cancel_accept();
	
	private:
	asio::io_service& _ios;
	std::vector<std::unique_ptr<OuiServiceImplementationServer>> _implementations;
	bool _is_listening;
	bool _is_canceling_listen;
	bool _is_accepting;
	bool _is_canceling_accept;
	std::vector<OuiServiceImplementationServer*> _listening_implementations;
};

class OuiServiceImplementationClient
{
	public:
	virtual ~OuiServiceImplementationClient() {}
	
	virtual GenericConnection connect(asio::yield_context yield) = 0;
	virtual void cancel_connect(asio::yield_context yield) = 0;
};

class OuiServiceClient
{
	public:
	OuiServiceClient(asio::io_service& ios);
	
	void add(std::unique_ptr<OuiServiceImplementationClient> implementation);
	
	GenericConnection connect(asio::yield_context yield);
	void cancel_connect();
	
	private:
	asio::io_service& _ios;
	std::vector<std::unique_ptr<OuiServiceImplementationServer>> _implementations;
};

} // ouinet namespace
