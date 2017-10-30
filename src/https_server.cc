#include "https_server.hh"

#include <csignal>

#include "Connection.hh"
#include "FileServer.hh"

namespace eweb {

https_server::https_server(asio::io_service& io_service,
               std::string address, std::string port,
               std::function<Reply(Request)> generator)
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

std::string https_server::get_password() {
  return "password";
}

asio::ssl::context& https_server::generate_context() {
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

void https_server::close() {
  acceptor.close();
  stop_all();
}

void https_server::do_accept() {
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

void https_server::setup_new_conn() {
  auto new_conn = std::make_shared<Connection<socket_t> >(
    std::move(socket), generator);

  connections.insert(new_conn);
  new_conn->set_on_close([this,new_conn]() {
      connections.erase(new_conn);
      new_conn->stop();
    });
  new_conn->start();
}

template<>
void Connection<https_server::socket_t>::init() {
  socket->async_handshake(
    asio::ssl::stream_base::server,
    [this](auto& error) {
      if(!error) {
        this->do_read();
      }
    });
}

void https_server::stop_all() {
  for(auto& conn : connections) {
    conn->stop();
  }
  connections.clear();
}

}
