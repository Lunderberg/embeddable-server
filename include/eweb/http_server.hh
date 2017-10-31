#pragma once

#include <functional>
#include <set>
#include <string>

#include <asio.hpp>

#include "Connection.hh"
#include "Response.hh"
#include "Request.hh"

namespace eweb {
  class http_server {
    typedef asio::ip::tcp::socket socket_t;
    //typedef asio::ssl::stream<asio::ip::tcp::socket> ssl_socket;

  public:
    http_server(asio::io_service& io_service,
                std::string address, std::string port,
                std::function<Response(Request)> generator)
      : io_service(io_service),
        acceptor(io_service),
        socket(nullptr),
        generator(generator)
      {
        asio::ip::tcp::resolver resolver(io_service);
        asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
        acceptor.open(endpoint.protocol());
        acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        acceptor.bind(endpoint);
        acceptor.listen();

        do_accept();
      }

    void stop_all() {
      for(auto& conn : connections) {
        conn->stop();
      }
      connections.clear();
    }


    void close() {
      acceptor.close();
      stop_all();
    }

  private:
    void do_accept() {
      if(!socket) {
        socket = std::make_unique<socket_t>(io_service);
      }

      acceptor.async_accept(
        *socket, [this](std::error_code ec) {
          if(!acceptor.is_open()) {
            return;
          }

          if(!ec) {
            setup_new_conn();
          }

          do_accept();
        });
    }

    void setup_new_conn() {
      auto new_conn = std::make_shared<Connection<socket_t> >(
        std::move(socket), generator);

      connections.insert(new_conn);
      new_conn->set_on_close([this,new_conn]() {
          connections.erase(new_conn);
          new_conn->stop();
        });
      new_conn->start();
    }

    asio::io_service& io_service;
    asio::ip::tcp::acceptor acceptor;
    std::unique_ptr<socket_t> socket;

    std::function<Response(Request)> generator;
    std::set<std::shared_ptr<Connection<socket_t> > > connections;
  };
}
