#ifndef _SERVER_OPTS_H_
#define _SERVER_OPTS_H_

#include <functional>
#include <string>

#include "asio.hpp"

#include "Response.hh"
#include "Request.hh"
#include "get_password_stdin.hh"

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

  /// The file containing SSL setup for the server.
  /**
     Should be a PEM format file containing the encrypted private SSL
     key, the signed certificate, and Diffie-Hellman parameters.
   */
  server_opts& ssl_file(std::string filename) {
    server_file_ = filename;
    return *this;
  }

  /// A function to return the encryption key for the private SSL key.
  /**
     Defaults to asking the user for the password on stdin.
   */
  server_opts& ssl_password_function(std::function<std::string()> getter) {
    get_server_password = getter;
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
  asio::io_service* io_service_to_use = nullptr;
  bool close_server_on_sigint = true;

  std::function<Response(Request)> http_generator;
  int http_listen_port = 80;

  std::function<Response(Request)> https_generator;
  int https_listen_port = 443;
  std::string server_file_ = "server.pem";
  std::function<std::string()> get_server_password = get_password_stdin;
};


}

#endif /* _SERVER_OPTS_H_ */
