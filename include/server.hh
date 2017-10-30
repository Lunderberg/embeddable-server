#ifndef _SERVER_H_
#define _SERVER_H_

#include <memory>

#include "asio.hpp"

#include "http_server.hh"
#include "https_server.hh"
#include "server_opts.hh"

namespace eweb {

class server {
public:
  /// Initialize the server, using the options given.
  server(const server_opts& opts);

  /// Close the server, and all current connections.
  void close();

  /// Run the server.
  void run() { io_service->run(); }

private:
  /// The io_service, either owned or external, that should always be used.
  asio::io_service* io_service;

  /// An owning pointer to an io_service, in case no external one was supplied.
  std::unique_ptr<asio::io_service> io_service_owning;

  std::unique_ptr<asio::signal_set> signals;


  std::unique_ptr<http_server> http;
  std::unique_ptr<https_server> https;
};

}

#endif /* _SERVER_H_ */
