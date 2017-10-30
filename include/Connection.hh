#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "asio.hpp"

#include "CommonReplies.hh"
#include "Reply.hh"
#include "Request.hh"
#include "RequestParser.hh"

namespace eweb {

template<typename SocketType>
class Connection : public std::enable_shared_from_this<Connection<SocketType> > {
public:
  Connection(std::unique_ptr<SocketType> socket,
             std::function<Reply(Request)> generate_reply)
    : socket(std::move(socket)), generate_reply(generate_reply) { }

  void start() { init(); }
  void stop() { socket->lowest_layer().close(); }

  void set_on_close(std::function<void()> callback) {
    on_close = callback;
  }

private:
  void init() {
    do_read();
  }

  void do_read()  {
    auto keep_alive = this->shared_from_this();
    socket->async_read_some(
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
          on_close();
        }
      });
  }


  void do_write() {
    auto keep_alive = this->shared_from_this();
    asio::async_write(
      *socket, reply.asio_buffers(),
      [this, keep_alive](std::error_code ec, std::size_t /*bytes_transferred*/) {
        if(!ec) {
          std::error_code ignored_ec;
          socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both,
                          ignored_ec);
        }

        if(ec != asio::error::operation_aborted && on_close) {
          on_close();
        }
      });
  }

  std::unique_ptr<SocketType> socket;

  std::function<void()> on_close;
  std::function<Reply(Request)> generate_reply;

  std::array<char, 8192> buffer;
  Reply reply;
};

}

#endif /* _CONNECTION_H_ */
