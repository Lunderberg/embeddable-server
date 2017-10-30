#pragma once

#include <functional>
#include <set>
#include <string>

#include <asio.hpp>
#include <asio/ssl.hpp>

#include "Connection.hh"
#include "Response.hh"
#include "Request.hh"

namespace eweb {
class https_server {
public:
  typedef asio::ssl::stream<asio::ip::tcp::socket> socket_t;

  https_server(asio::io_service& io_service,
               std::string address, std::string port,
               std::function<Response(Request)> generator)
    : io_service(io_service),
      acceptor(io_service),
      context(io_service, asio::ssl::context::sslv23),
      socket(nullptr),
      generator(generator)
    {
      generate_context();

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
      socket = std::make_unique<socket_t>(io_service, context);
    }

    acceptor.async_accept(
      socket->lowest_layer(), [this](std::error_code ec) {
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

  std::string get_password() {
    return "password";
  }

  asio::ssl::context& generate_context() {
    context.set_options(
      asio::ssl::context::default_workarounds |
      asio::ssl::context::no_sslv2 |
      asio::ssl::context::single_dh_use);
    context.set_password_callback(
      [this](size_t /*max_length*/,
             asio::ssl::context::password_purpose /*purpose*/) {
        return get_password();
      });
    context.use_certificate_chain_file("server.pem");
    context.use_private_key_file("server.pem", asio::ssl::context::pem);
    context.use_tmp_dh_file("server.pem");

    return context;
  }

  asio::io_service& io_service;
  asio::ip::tcp::acceptor acceptor;
  asio::ssl::context context;
  std::unique_ptr<socket_t> socket;

  std::function<Response(Request)> generator;
  std::set<std::shared_ptr<Connection<socket_t> > > connections;
};

/// Alternate initialization for SSL sockets, to do handshake first.
template<>
inline void Connection<https_server::socket_t>::init() {
  socket->async_handshake(
    asio::ssl::stream_base::server,
    [this](auto& error) {
      if(!error) {
        this->do_read();
      }
    });
}

}
