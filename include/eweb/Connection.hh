#pragma once

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "asio.hpp"

#include "CommonResponses.hh"
#include "Response.hh"
#include "Request.hh"
#include "RequestParser.hh"

namespace eweb {

template<typename SocketType>
class Connection : public std::enable_shared_from_this<Connection<SocketType> > {
public:
  Connection(std::unique_ptr<SocketType> socket,
             std::function<Response(Request)> generate_response)
    : socket(std::move(socket)), generate_response(generate_response) { }

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
            response = generate_response(request);
          } else if(request.parse_result == bad) {
            response = common_response(Response::bad_request);
          }

          response.headers["Content-Length"] = std::to_string(response.content.size());
          do_write();

        } else if (ec != asio::error::operation_aborted && on_close) {
          on_close();
        }
      });
  }


  void do_write() {
    auto keep_alive = this->shared_from_this();
    asio::async_write(
      *socket, response.asio_buffers(),
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
  std::function<Response(Request)> generate_response;

  std::array<char, 8192> buffer;
  Response response;
};

}
