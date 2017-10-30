#ifndef _FILESERVER_H_
#define _FILESERVER_H_

#include <string>

#include "Reply.hh"
#include "Request.hh"

namespace eweb {

class file_server {
public:
  file_server(std::string root_path);

  Reply operator()(Request request);

private:
  std::string root_path;
};

}

#endif /* _FILESERVER_H_ */
