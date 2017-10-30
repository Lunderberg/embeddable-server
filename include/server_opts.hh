#ifndef _SERVER_OPTS_H_
#define _SERVER_OPTS_H_

#include <functional>
#include <string>

#include "asio.hpp"

#include "Response.hh"
#include "Request.hh"

namespace eweb {

class server_opts {
public:
  /// Default constructor
  server_opts() { }

  /// Set the address for the server to bind to
  /**
     If unspecified, defaults to "localhost"
   */
  server_opts& address(std::string ip_address) {
    bind_to_address = ip_address;
    return *this;
  }

  /// The port on which the http server should run.
  /**
     If unspecified, defaults to 80.
   */
  server_opts& http_port(int port) {
    http_listen_port = port;
    return *this;
  }

  /// The port on which the https server should run.
  /**
     If unspecified, defaults to 443.
   */
  server_opts& https_port(int port) {
    https_listen_port = port;
    return *this;
  }

  /// The callback used to generate http response.
  /**
     If unspecified, the http server will not run.
   */
  server_opts& http_callback(std::function<Response(Request)> gen) {
    http_generator = gen;
    return *this;
  }

  /// The callback used to generate https response.
  /**
     If unspecified, the https server will not run.

     If this option has been used, and SSL support has not been
     enabled with the -DEWEB_ENABLE_SSL, constructor of eweb::server
     will throw.
   */
  server_opts& https_callback(std::function<Response(Request)> gen) {
    https_generator = gen;
    return *this;
  }

  /// Whether the server should catch and close cleanly on sigint.
  /**
     Defaults to true.
   */
  server_opts& close_on_sigint(bool close) {
    close_server_on_sigint = close;
    return *this;
  }

  /// The io_service to run on.
  /**
     If specified here, the server will use an existing
     asio::io_service.  In this case, the server does not own the
     io_service.

     If no io_service is specified, the server will create and own an
     io_service.
   */
  server_opts& io_service(asio::io_service& service) {
    // Deliberately taking as reference, not pointer, to make it clear
    // that we do not own this service.
    io_service_to_use = &service;
    return *this;
  }

private:
  friend class server;

  std::string bind_to_address = "localhost";
  int http_listen_port = 80;
  int https_listen_port = 443;
  std::function<Response(Request)> http_generator;
  std::function<Response(Request)> https_generator;
  asio::io_service* io_service_to_use;
  bool close_server_on_sigint = true;
};


}

#endif /* _SERVER_OPTS_H_ */
