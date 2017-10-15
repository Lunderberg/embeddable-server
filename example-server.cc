#include <iostream>

#include "server.hh"

int main(int argc, char** argv) {
  try {
    if(argc != 4) {
      std::cerr << "Usage: example-server <address> <port> <root>\n"
                << "  For IPv4, try\n"
                << "     example-server 0.0.0.0 80 .\n"
                << "  For IPv6, try\n"
                << "     example-server 0::0 80 .\n";
      return 1;
    }

    http::server server(argv[1], argv[2], argv[3]);
    server.run();

  } catch (std::exception& e) {
    std::cerr << "exception: " << e.what() << std::endl;
    return 2;
  }

  return 0;
}
