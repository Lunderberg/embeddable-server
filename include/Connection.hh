#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "asio.hpp"

#include "Reply.hh"
#include "Request.hh"

namespace http {
  class Connection : public std::enable_shared_from_this<Connection> {
  public:
    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    Connection(asio::ip::tcp::socket socket,
               std::function<Reply(Request)> generate_reply);

    void start();
    void stop();

    void set_on_close(std::function<void(Connection*)> callback) {
      on_close = callback;
    }

  private:
    void do_read();
    void do_write();

    asio::ip::tcp::socket socket;

    std::function<void(Connection*)> on_close;
    std::function<Reply(Request)> generate_reply;

    std::array<char, 8192> buffer;
    Reply reply;
  };
}

#endif /* _CONNECTION_H_ */
