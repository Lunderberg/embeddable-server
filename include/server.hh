#ifndef _SERVER_H_
#define _SERVER_H_

#include <functional>
#include <map>
#include <string>

#include "asio.hpp"

#include "Connection.hh"
#include "Reply.hh"
#include "Request.hh"

namespace http {
  class server {
  public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(std::string address, std::string port, std::string root_path);
    explicit server(std::string address, std::string port,
                    std::function<Reply(Request)> generator);

    void run();

    void stop_all();

  private:
    void do_wait_for_signal();
    void do_accept();

    void setup_new_conn();

    asio::io_service io_service;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;

    std::function<Reply(Request)> generator;
    std::map<Connection*, std::shared_ptr<Connection> > connections;
  };
}

#endif /* _SERVER_H_ */
