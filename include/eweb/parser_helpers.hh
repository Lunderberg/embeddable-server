#pragma once

namespace eweb {

inline bool is_ascii_char(int c) {
  return (c >= 0) && (c <= 127);
}

inline bool is_ascii_control(int c) {
  return (c >= 0 && c <= 31) || (c == 127);
}

inline bool is_ascii_special(int c) {
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

inline bool is_ascii_digit(int c) {
  return (c >= '0') && (c <= '9');
}

inline bool is_regular(int c) {
  return is_ascii_char(c) && !is_ascii_control(c) && !is_ascii_special(c);
}

}
