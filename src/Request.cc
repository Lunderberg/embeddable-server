#include "Request.hh"

namespace eweb {

std::ostream& operator<<(std::ostream& out, parse_result_type res) {
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

std::ostream& operator<<(std::ostream& out, const Request& res) {
  out << "Status: " << res.parse_result << "\n"
      << res.method << " " << res.uri
      << " HTTP/" << res.http_version_major << "." << res.http_version_minor << "\n";

  for(const auto& header : res.headers) {
    out << header.first << ": " << header.second << "\n";
  }

  return out;
}

}
