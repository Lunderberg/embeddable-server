#include "ConnectionManager.hh"

namespace http {

void ConnectionManager::start(std::shared_ptr<Connection> conn) {
  connections.insert(conn);
  conn->set_on_close([this, conn](Connection*){
      connections.erase(conn);
      conn->stop();
    });
  conn->start();
}

void ConnectionManager::stop_all() {
  for(auto& conn: connections) {
    conn->stop();
  }
  connections.clear();
}

}
