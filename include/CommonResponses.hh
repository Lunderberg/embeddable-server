#pragma once

#include <sstream>

#include "Response.hh"

namespace eweb {

inline Response common_response(Response::ResponseCode code) {
  std::string error;
  switch(code) {
    case Response::ok:  error = "OK"; break;
    case Response::created: error = "Created"; break;
    case Response::accepted: error = "Accepted"; break;
    case Response::no_content: error = "No Content"; break;
    case Response::multiple_choices: error = "Multiple Choices"; break;
    case Response::moved_permanently: error = "Moved Permanently"; break;
    case Response::moved_temporarily: error = "Moved Temporarily"; break;
    case Response::not_modified: error = "Not Modified"; break;
    case Response::bad_request: error = "Bad Request"; break;
    case Response::unauthorized: error = "Unauthorized"; break;
    case Response::forbidden: error = "Forbidden"; break;
    case Response::not_found: error = "Not Found"; break;
    case Response::internal_server_error: error = "Internal Server Error"; break;
    case Response::not_implemented: error = "Not Implemented"; break;
    case Response::bad_gateway: error = "Bad Gateway"; break;
    case Response::service_unavailable: error = "Service Unavailable"; break;

    default:
      error = "Internal Server Error";
      code = Response::internal_server_error;
      break;

  };

  std::ostringstream ss;
  ss << "<html>"
     << "<head><title>" << error << "</title></head>"
     << "<body><h1>" << int(code) << " " << error << "</h1></body>"
     << "</html>";

  Response output;

  output.response_code = code;
  output.content = ss.str();

  output.headers["Content-Length"] = std::to_string(output.content.size());
  output.headers["Content-Type"] = "text/html";

  return output;
}

}
