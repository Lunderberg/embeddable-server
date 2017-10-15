#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>

#include "asio.hpp"

#include "ConnectionManager.hh"

namespace http {
  class server {
  public:
    server(const server&) = delete;
    server& operator=(const server&) = delete;

    explicit server(std::string address, std::string port, std::string root_path);

    void run();

  private:
    void do_wait_for_signal();
    void do_accept();

    asio::io_service io_service;
    asio::signal_set signals;
    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;

    ConnectionManager connection_manager;
  };
}

#endif /* _SERVER_H_ */
