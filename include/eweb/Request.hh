#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace eweb {

enum parse_result_type { good, bad, indeterminate };

struct Request {
  parse_result_type parse_result;
  std::string method;
  std::string uri;
  int http_version_major = 0;
  int http_version_minor = 0;
  std::map<std::string, std::string> headers;
};

inline std::ostream& operator<<(std::ostream& out, parse_result_type res) {
  switch(res) {
    case good:
      out << "good";
      break;

    case bad:
      out << "bad";
      break;

    case indeterminate:
      out << "indeterminate";
      break;

    default:
      out << "unknown";
      break;
  }

  return out;
}

inline std::ostream& operator<<(std::ostream& out, const Request& res) {
  out << "Status: " << res.parse_result << "\n"
      << res.method << " " << res.uri
      << " HTTP/" << res.http_version_major << "." << res.http_version_minor << "\n";

  for(const auto& header : res.headers) {
    out << header.first << ": " << header.second << "\n";
  }

  return out;
}

}
