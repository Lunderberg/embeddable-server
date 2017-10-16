#include "server.hh"

#include <csignal>

#include "Connection.hh"

http::server::server(std::string address, std::string port, std::string root_path)
  : io_service(),
    signals(io_service),
    acceptor(io_service),
    socket(io_service)
{
  signals.add(SIGINT);
  signals.add(SIGTERM);

  do_wait_for_signal();

  asio::ip::tcp::resolver resolver(io_service);
  asio::ip::tcp::endpoint endpoint = *resolver.resolve({address, port});
  acceptor.open(endpoint.protocol());
  acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
  acceptor.bind(endpoint);
  acceptor.listen();

  do_accept();
}

void http::server::run() {
  io_service.run();
}

void http::server::do_wait_for_signal() {
  signals.async_wait(
    [this](std::error_code ec, int signo) {
      acceptor.close();
      stop_all();
    });
}

void http::server::do_accept() {
  acceptor.async_accept(
    socket, [this](std::error_code ec) {
      if(!acceptor.is_open()) {
        return;
      }

      if(!ec) {
        auto new_conn = std::make_shared<Connection>(std::move(socket));
        connections[new_conn.get()] = new_conn;
        new_conn->set_on_close([this](Connection* conn) {
            connections.erase(conn);
            conn->stop();
          });
        new_conn->start();
      }

      do_accept();
    });
}

void http::server::stop_all() {
  for(auto& conn : connections) {
    conn.second->stop();
  }
  connections.clear();
}
