#include "RequestParser.hh"

namespace {

bool is_ascii_char(int c) {
  return (c >= 0) && (c <= 127);
}

bool is_ascii_control(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

bool is_ascii_special(int c) {
  switch(c)
  {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
      return true;
    default:
      return false;
  }
}

bool is_ascii_digit(int c) {
  return (c >= '0') && (c <= '9');
}

bool is_regular(int c) {
  return is_ascii_char(c) && !is_ascii_control(c) && !is_ascii_special(c);
}

}

namespace http {

parse_result_type RequestParser::consume(char input) {
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
      if(input == '\r') {
        state = expecting_newline_3;
        return indeterminate;
      } else if(output.headers.empty() && (input == ' ' || input == '\t')) {
        state = header_lws;
        return indeterminate;
      } else if(!is_regular(input)) {
        return bad;
      } else {
        output.headers.push_back(Header());
        output.headers.back().name.push_back(input);
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
        output.headers.back().value.push_back(input);
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
        output.headers.back().name.push_back(input);
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
        state = expecting_newline_2;
        return indeterminate;
      } else if(is_ascii_control(input)) {
        return bad;
      } else {
        output.headers.back().value.push_back(input);
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

}
