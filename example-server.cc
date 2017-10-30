#include <iostream>

#include "http_server.hh"
#include "https_server.hh"
#include "FileServer.hh"

int main(int argc, char** argv) {
  try {
    // if(argc != 4) {
    //   std::cerr << "Usage: example-server <address> <port> <root>\n"
    //             << "  For IPv4, try\n"
    //             << "     example-server 0.0.0.0 80 .\n"
    //             << "  For IPv6, try\n"
    //             << "     example-server 0::0 80 .\n";
    //   return 1;
    // }

    asio::io_service io_service;

    eweb::http_server http_server(io_service, "localhost", "12345", eweb::FileServer("."));
    eweb::https_server https_server(io_service, "localhost", "12346", eweb::FileServer("."));

    asio::signal_set signals(io_service);
    signals.add(SIGINT);
    signals.add(SIGTERM);
    signals.async_wait(
      [&](std::error_code /*ec*/, int /*signo*/) {
        http_server.close();
        https_server.close();
      });

    io_service.run();

  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
