#ifndef _HTTPS_SERVER_H_
#define _HTTPS_SERVER_H_

#include <functional>
#include <set>
#include <string>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "Connection.hh"
#include "Reply.hh"
#include "Request.hh"

namespace http {
  class https_server {
  public:
    typedef asio::ssl::stream<asio::ip::tcp::socket> socket_t;

    https_server(asio::io_service& io_service,
                 std::string address, std::string port,
                 std::function<Reply(Request)> generator);

    void run();
    void stop_all();
    void close();

  private:
    void do_wait_for_signal();
    void do_accept();

    void setup_new_conn();

    std::string get_password();

    asio::ssl::context& generate_context();

    asio::io_service& io_service;
    asio::ip::tcp::acceptor acceptor;
    asio::ssl::context context;
    std::unique_ptr<socket_t> socket;

    std::function<Reply(Request)> generator;
    std::set<std::shared_ptr<Connection<socket_t> > > connections;
  };
}

#endif /* _HTTPS_SERVER_H_ */
