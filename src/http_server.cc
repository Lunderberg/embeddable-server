#include "http_server.hh"

#include <csignal>

#include "Connection.hh"

namespace eweb {

http_server::http_server(asio::io_service& io_service,
               std::string address, std::string port,
               std::function<Reply(Request)> generator)
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

void http_server::close() {
  acceptor.close();
  stop_all();
}

void http_server::do_accept() {
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

void http_server::setup_new_conn() {
  auto new_conn = std::make_shared<Connection<socket_t> >(
    std::move(socket), generator);

  connections.insert(new_conn);
  new_conn->set_on_close([this,new_conn]() {
      connections.erase(new_conn);
      new_conn->stop();
    });
  new_conn->start();
}

void http_server::stop_all() {
  for(auto& conn : connections) {
    conn->stop();
  }
  connections.clear();
}

}
