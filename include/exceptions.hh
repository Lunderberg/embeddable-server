#pragma once

#include <stdexcept>

namespace eweb {

struct eweb_error : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

struct ssl_disabled : public eweb_error {
  using eweb_error::eweb_error;
};

}
