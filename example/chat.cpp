#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include <iostream>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/write.hpp>

#include "generic_connection.h"
#include <i2poui.h>

using namespace std;
namespace asio = boost::asio;

using namespace i2poui;

std::shared_ptr<Server> server;
static string remove_new_line(string s)
{
    while (!s.empty() && *(--s.end()) == '\n') {
        s.resize(s.size() - 1);
    }
    return s;
}

static string consume(asio::streambuf& buf, size_t n)
{
    string out(n, '\0');
    buf.sgetn((char*) out.c_str(), n);
    return out;
}

static void run_chat(const boost::system::error_code& ec, ouinet::GenericConnection&  connection) {
  auto& ios = connection.get_io_service();

  asio::spawn(ios, [] (asio::yield_context yield) { return;});
  
    // This co-routine reads always from the socket and write it to std out.
      asio::spawn(ios, [&connection] (asio::yield_context yield) {
            boost::system::error_code ec;
            asio::streambuf buffer(512);

            while (true) {
                size_t n = asio::async_read_until(connection, buffer, '\n', yield[ec]);

                if (ec) return;

                cout << "Received: "
                     << remove_new_line(consume(buffer, n))
                     << endl;
            }
        });

    // This co-routine reads from std input and send it to peer
    asio::spawn(ios, [&ios, &connection] (auto yield) {
            boost::system::error_code ec;
            asio::posix::stream_descriptor input(ios, ::dup(STDIN_FILENO)); 

            asio::streambuf buffer(512);

            cout << "Enter a message to the peer" << endl;
            while (true) {
              boost::system::error_code ec;
              size_t size = asio::async_read_until(input, buffer, '\n',
                                                   yield[ec]);
              if (ec) break;

              cout << "sending your message..." << endl;
              if (size > 0) 
                asio::async_write(connection, asio::buffer(consume(buffer, buffer.size())), yield[ec]);
            }
      });
}

static void connect_and_run_chat( Service& service
                                , string target_id
                                , asio::yield_context yield)
{
  boost::system::error_code ec;
  cout << "Connecting to " << target_id << endl;
  auto connector = service.build_connector(target_id, "");

  //connector->wait_to_get_ready(yield);

  //cout << "connector tunnel to the server is established" << endl;

  auto connection = connector->connect(yield);

 cout << "connection to the server is established" << endl;

  run_chat(ec, connection);
}

static void accept_and_run_chat( Service& service
                                 , asio::yield_context yield)
{
  boost::system::error_code ec;
  auto server = service.build_acceptor("private_key");
        
  cout << "Accepting on " << server->public_identity() << endl;

  cout << "Acceptor has been built" << endl;

  cout << "Acceptor tunnel is established" << endl;

  //acceptor->accept(run_chat);
  auto connection = server->accept(yield[ec]);
  
  run_chat(ec, connection);
  // cout << "we are here" << endl;
}

static void print_usage(const char* app_name)
{
    cerr << "Usage:\n";
    cerr << "    " << app_name << " <homedir> [peer-id]\n";
    cerr << "If [peer-id] is used the app acts as a client, "
            "otherwise it acts as a server\n";
}

int main(int argc, char* const* argv)
{
    if (argc != 2 && argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    asio::io_service ios;

    bool is_client = argc >= 3;

    Service service(argv[1], ios);

    asio::spawn(ios, [&] (asio::yield_context yield) {
            if (is_client) {
              connect_and_run_chat(service, argv[2], yield);
            }
            else {
              accept_and_run_chat(service, yield);
            }
        });

    ios.run();
}
