#include "Connection.hh"

#include <iostream>

#include "RequestParser.hh"

namespace http{

Connection::Connection(ConnectionManager& connection_manager,
                             asio::ip::tcp::socket socket)
  : connection_manager(connection_manager), socket(std::move(socket)) { }

void Connection::start() {
  std::cout << "Starting connection's first do_read" << std::endl;
  do_read();
}

void Connection::stop() {
  std::cout << "Closing connection" << std::endl;
  socket.close();
}

void Connection::do_read() {
  auto shared_this = shared_from_this();
  socket.async_read_some(
    asio::buffer(buffer),
    [this, shared_this](std::error_code ec, std::size_t bytes_transferred) {
      if(!ec) {

        std::string data_received(buffer.data(), bytes_transferred);
        Request request = parse_request(buffer.data(), buffer.data()+bytes_transferred);

        if(request.parse_result == good) {
          reply = handle_request(request);
          do_write();
        } else if(request.parse_result == bad) {
          // TODO: Some stock response to bad requests.
          // do_write();
        } else {
          do_read();
        }

      } else if (ec != asio::error::operation_aborted) {
        connection_manager.stop(shared_from_this());
      }
    });
}

void Connection::do_write() {
  auto shared_this = shared_from_this();
  asio::async_write(
    socket, reply.asio_buffers(),
    [this, shared_this](std::error_code ec, std::size_t /*bytes_transferred*/) {
      if(!ec) {
        std::error_code ignored_ec;
        socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                        ignored_ec);
      }

      if(ec != asio::error::operation_aborted) {
        connection_manager.stop(shared_this);
      }
    });
}

Reply Connection::handle_request(const Request& request) {
  Reply reply;

  reply.response_code = Reply::ok;
  reply.content = "Test data here";

  reply.headers.resize(2);

  reply.headers[0].name = "Content-Length";
  reply.headers[0].value = std::to_string(reply.content.size());
  reply.headers[1].name = "Content-Type";
  reply.headers[1].value = "text/html";

  return reply;
}

}
