#include "CommonReplies.hh"

#include <sstream>

namespace eweb {

Reply common_reply(Reply::ResponseCode code) {
  std::string error;
  switch(code) {
    case Reply::ok:  error = "OK"; break;
    case Reply::created: error = "Created"; break;
    case Reply::accepted: error = "Accepted"; break;
    case Reply::no_content: error = "No Content"; break;
    case Reply::multiple_choices: error = "Multiple Choices"; break;
    case Reply::moved_permanently: error = "Moved Permanently"; break;
    case Reply::moved_temporarily: error = "Moved Temporarily"; break;
    case Reply::not_modified: error = "Not Modified"; break;
    case Reply::bad_request: error = "Bad Request"; break;
    case Reply::unauthorized: error = "Unauthorized"; break;
    case Reply::forbidden: error = "Forbidden"; break;
    case Reply::not_found: error = "Not Found"; break;
    case Reply::internal_server_error: error = "Internal Server Error"; break;
    case Reply::not_implemented: error = "Not Implemented"; break;
    case Reply::bad_gateway: error = "Bad Gateway"; break;
    case Reply::service_unavailable: error = "Service Unavailable"; break;

    default:
      error = "Internal Server Error";
      code = Reply::internal_server_error;
      break;

  };

  std::ostringstream ss;
  ss << "<html>"
     << "<head><title>" << error << "</title></head>"
     << "<body><h1>" << int(code) << " " << error << "</h1></body>"
     << "</html>";

  Reply output;

  output.response_code = code;
  output.content = ss.str();

  output.headers["Content-Length"] = std::to_string(output.content.size());
  output.headers["Content-Type"] = "text/html";

  return output;
}

}
