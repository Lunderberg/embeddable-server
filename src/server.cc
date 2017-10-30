#include "server.hh"

namespace eweb {

server::server(const server_opts& opts) {
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
      opts.https_generator);
  }
}

void server::close() {
  if(http) {
    http->close();
  }

  if(https) {
    https->close();
  }
}

}
