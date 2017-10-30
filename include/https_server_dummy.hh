#pragma once

#include <functional>
#include <string>

#include <asio.hpp>

#include "Response.hh"
#include "Request.hh"
#include "exceptions.hh"

namespace eweb {
class https_server {
public:
  enum { enabled = 0 };

  https_server(
    asio::io_service& /*io_service*/,
    std::string /*address*/, std::string /*port*/,
    std::function<Response(Request)> /*generator*/,
    std::string /*server_file*/,
    std::function<std::string()> /*get_password*/ = get_password_stdin) {

    throw ssl_disabled("SSL support was not enabled.  "
                       "Recompile with -DEWEB_ENABLE_SSL -lssl -lcrypto");
  }

  void stop_all() { }

  void close() { }
};

}
