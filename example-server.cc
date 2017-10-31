#include <iostream>

#include "eweb.hh"

int main() {
  try {
    eweb::server_opts config;
    config.address("localhost");

    if(eweb::https_server::enabled) {
      config.http_port(12345);
      config.redirect_http_to_https();
      config.https_port(12346);
      config.https_callback(eweb::file_server("."));

    } else {
      config.http_port(12345);
      config.http_callback(eweb::file_server("."));
    }

    eweb::server server(config);

    server.run();

  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
