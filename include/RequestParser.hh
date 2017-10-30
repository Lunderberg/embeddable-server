#ifndef _REQUESTPARSER_H_
#define _REQUESTPARSER_H_

#include "Request.hh"
#include "parser_helpers.hh"

namespace eweb {

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
  parse_result_type consume(char input) {
    switch(state) {
      case method_start:
        if(!is_regular(input)) {
          return bad;
        } else {
          state = method;
          output.method.push_back(input);
          return indeterminate;
        }
        break;

      case method:
        if(input == ' ') {
          state = uri;
          return indeterminate;
        } else if(!is_regular(input)) {
          return bad;
        } else {
          output.method.push_back(input);
          return indeterminate;
        }
        break;

      case uri:
        if(input == ' ') {
          state = http_version_h;
          return indeterminate;
        } else if (is_ascii_control(input)) {
          return bad;
        } else {
          output.uri.push_back(input);
          return indeterminate;
        }
        break;

      case http_version_h:
        if(input == 'H') {
          state = http_version_t_1;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_t_1:
        if(input == 'T') {
          state = http_version_t_2;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_t_2:
        if(input == 'T') {
          state = http_version_p;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_p:
        if(input == 'P') {
          state = http_version_slash;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_slash:
        if(input == '/') {
          output.http_version_major = 0;
          output.http_version_minor = 0;
          state = http_version_major_start;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_major_start:
        if(is_ascii_digit(input)) {
          output.http_version_major = (input - '0');
          state = http_version_major;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_major:
        if(input == '.') {
          state = http_version_minor_start;
          return indeterminate;
        } else if(is_ascii_digit(input)) {
          output.http_version_major = 10*output.http_version_major + (input - '0');
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_minor_start:
        if(is_ascii_digit(input)) {
          output.http_version_minor = (input - '0');
          state = http_version_minor;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case http_version_minor:
        if(input == '\r') {
          state = expecting_newline_1;
          return indeterminate;
        } else if(is_ascii_digit(input)) {
          output.http_version_minor = 10*output.http_version_minor + (input - '0');
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case expecting_newline_1:
        if(input == '\n') {
          state = header_line_start;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case header_line_start:
        current_header_key.clear();
        current_header_value.clear();
        if(input == '\r') {
          state = expecting_newline_3;
          return indeterminate;
        } else if(output.headers.empty() && (input == ' ' || input == '\t')) {
          state = header_lws;
          return indeterminate;
        } else if(!is_regular(input)) {
          return bad;
        } else {
          current_header_key.push_back(input);
          state = header_name;
          return indeterminate;
        }
        break;

      case header_lws:
        if(input == '\r') {
          state = expecting_newline_2;
          return indeterminate;
        } else if(input == ' ' || input == '\t') {
          return indeterminate;
        } else if(is_ascii_control(input)) {
          return bad;
        } else {
          state = header_value;
          current_header_key.push_back(input);
          return indeterminate;
        }
        break;

      case header_name:
        if(input == ':') {
          state = space_before_header_value;
          return indeterminate;
        } else if(!is_regular(input)) {
          return bad;
        } else {
          current_header_key.push_back(input);
          return indeterminate;
        }
        break;

      case space_before_header_value:
        if(input == ' ') {
          state = header_value;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case header_value:
        if(input == '\r') {
          if(output.headers.count(current_header_key)) {
            output.headers[current_header_key] = (output.headers[current_header_key] +
                                                  ", " +
                                                  current_header_value);
          } else {
            output.headers[current_header_key] = current_header_value;
          }
          state = expecting_newline_2;
          return indeterminate;
        } else if(is_ascii_control(input)) {
          return bad;
        } else {
          current_header_value.push_back(input);
          return indeterminate;
        }
        break;

      case expecting_newline_2:
        if(input == '\n') {
          state = header_line_start;
          return indeterminate;
        } else {
          return bad;
        }
        break;

      case expecting_newline_3:
        if(input == '\n') {
          return good;
        } else {
          return bad;
        }
        break;

      default:
        return bad;

    }
  }

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
