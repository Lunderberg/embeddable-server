#pragma once

#include <filesystem>

#include "Response.hh"

namespace eweb {

Response directory_listing(std::string full_path) {
  Response output;

  output.response_code = 200;
  output.content = "asdfasdfasdf";

  return output;
}

}
