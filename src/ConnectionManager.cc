#include "ConnectionManager.hh"

#include "Connection.hh"

namespace http {

void ConnectionManager::start(std::shared_ptr<Connection> conn) {
  connections.insert(conn);
  conn->start();
}

void ConnectionManager::stop(std::shared_ptr<Connection> conn) {
  connections.erase(conn);
  conn->stop();
}

void ConnectionManager::stop_all() {
  for(auto& conn: connections) {
    conn->stop();
  }
  connections.clear();
}

}
