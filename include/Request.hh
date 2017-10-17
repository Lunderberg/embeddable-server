#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace http {

enum parse_result_type { good, bad, indeterminate };

struct Request {
  parse_result_type parse_result;
  std::string method;
  std::string uri;
  int http_version_major = 0;
  int http_version_minor = 0;
  std::map<std::string, std::string> headers;
};

std::ostream& operator<<(std::ostream& out, parse_result_type res);

std::ostream& operator<<(std::ostream& out, const Request& res);

}

#endif /* _REQUEST_H_ */
