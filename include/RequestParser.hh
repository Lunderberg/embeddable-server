#ifndef _REQUESTPARSER_H_
#define _REQUESTPARSER_H_

#include "Request.hh"

namespace http {

class RequestParser {
public:
  RequestParser() : state(method_start) { }

  template<typename InputIterator>
  Request parse(InputIterator begin, InputIterator end) {
    while(begin != end) {
      auto result = consume(*begin++);
      if (result == good || result == bad) {
        output.parse_result = result;
        return output;
      }
    }

    output.parse_result = indeterminate;
    return output;
  }

private:
  parse_result_type consume(char input);

  Request output;

  enum State
  {
    method_start,
    method,
    uri,
    http_version_h,
    http_version_t_1,
    http_version_t_2,
    http_version_p,
    http_version_slash,
    http_version_major_start,
    http_version_major,
    http_version_minor_start,
    http_version_minor,
    expecting_newline_1,
    header_line_start,
    header_lws,
    header_name,
    space_before_header_value,
    header_value,
    expecting_newline_2,
    expecting_newline_3
  };

  State state;
  std::string current_header_key;
  std::string current_header_value;
};

template<typename InputIterator>
Request parse_request(InputIterator begin, InputIterator end) {
  RequestParser request_parser;
  return request_parser.parse(begin, end);
}

}

#endif /* _REQUESTPARSER_H_ */
