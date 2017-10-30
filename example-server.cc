#include <iostream>

#include "eweb.hh"

int main() {
  try {
    eweb::server server(eweb::server_opts()
                        .address("localhost")
                        .http_port(12345)
                        .http_callback(eweb::file_server("."))

                        .https_port(12346)
                        .https_callback(eweb::file_server("."))
    );

    server.run();

  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
