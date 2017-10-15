#ifndef _HEADER_H_
#define _HEADER_H_

#include <string>

namespace http {

struct Header {
  std::string name;
  std::string value;
};

}

#endif /* _HEADER_H_ */
