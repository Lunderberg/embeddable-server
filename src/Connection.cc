#include "Connection.hh"

#include "CommonReplies.hh"
#include "RequestParser.hh"

namespace http{

Connection::Connection(asio::ip::tcp::socket socket,
                       std::function<Reply(Request)> generate_reply)
  : socket(std::move(socket)), generate_reply(generate_reply) { }

void Connection::start() {
  do_read();
}

void Connection::stop() {
  socket.close();
}

void Connection::do_read() {
  auto keep_alive = shared_from_this();
  socket.async_read_some(
    asio::buffer(buffer),
    [this, keep_alive](std::error_code ec, std::size_t bytes_transferred) {
      if(!ec) {

        std::string data_received(buffer.data(), bytes_transferred);
        Request request = parse_request(buffer.data(), buffer.data()+bytes_transferred);

        if(request.parse_result != good and request.parse_result != bad) {
          do_read();
          return;
        }

        if(request.parse_result == good) {
          reply = generate_reply(request);
        } else if(request.parse_result == bad) {
          reply = common_reply(Reply::bad_request);
        }

        reply.headers["Content-Length"] = std::to_string(reply.content.size());
        do_write();

      } else if (ec != asio::error::operation_aborted && on_close) {
        on_close(this);
      }
    });
}

void Connection::do_write() {
  auto keep_alive = shared_from_this();
  asio::async_write(
    socket, reply.asio_buffers(),
    [this, keep_alive](std::error_code ec, std::size_t /*bytes_transferred*/) {
      if(!ec) {
        std::error_code ignored_ec;
        socket.shutdown(asio::ip::tcp::socket::shutdown_both,
                        ignored_ec);
      }

      if(ec != asio::error::operation_aborted && on_close) {
        on_close(this);
      }
    });
}

}
