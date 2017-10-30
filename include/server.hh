#ifndef _SERVER_H_
#define _SERVER_H_

#include <memory>

#include "asio.hpp"

#include "http_server.hh"
#include "server_opts.hh"

#ifdef EWEB_ENABLE_SSL
#include "https_server.hh"
#else
#include "https_server_dummy.hh"
#endif

namespace eweb {

class server {
public:
  /// Initialize the server, using the options given.
  server(const server_opts& opts)
    : io_service(nullptr) {
    if(opts.io_service_to_use) {
      io_service = opts.io_service_to_use;
    } else {
      io_service_owning = std::make_unique<asio::io_service>();
      io_service = io_service_owning.get();
    }

    if(opts.close_server_on_sigint) {
      signals = std::make_unique<asio::signal_set>(*io_service);
      signals->add(SIGINT);
      signals->add(SIGTERM);
      signals->async_wait(
        [this](std::error_code /*ec*/, int /*signo*/) {
          close();
        });
    }

    if(opts.http_generator) {
      http = std::make_unique<http_server>(
        *io_service,
        opts.bind_to_address,
        std::to_string(opts.http_listen_port),
        opts.http_generator);
    }

    if(opts.https_generator) {
      https = std::make_unique<https_server>(
        *io_service,
        opts.bind_to_address,
        std::to_string(opts.https_listen_port),
        opts.https_generator,
        opts.server_file_,
        opts.get_server_password);
    }
  }

  /// Close the server, and all current connections.
  void close() {
    if(http) {
      http->close();
    }

    if(https) {
      https->close();
    }
  }

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
