#ifndef _CONNECTIONMANAGER_H_
#define _CONNECTIONMANAGER_H_

#include <memory>
#include <set>

namespace http {
  class Connection;

  class ConnectionManager {
  public:
    ConnectionManager(const ConnectionManager&) = delete;
    ConnectionManager& operator=(const ConnectionManager&) = delete;

    ConnectionManager() { }

    void start(std::shared_ptr<Connection> conn);
    void stop(std::shared_ptr<Connection> conn);

    void stop_all();

  private:
    std::set<std::shared_ptr<Connection> > connections;
  };
}

#endif /* _CONNECTIONMANAGER_H_ */
