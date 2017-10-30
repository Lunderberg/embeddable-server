#include "file_server.hh"

#include <fstream>

#include "CommonResponses.hh"
#include "URIInfo.hh"

namespace eweb {

file_server::file_server(std::string root_path)
  : root_path(root_path) { }

Response file_server::operator()(Request request) {
  URIInfo info(request.uri);

  if(!info.is_valid) {
    return common_response(Response::bad_request);
  }

  std::string full_path = root_path + info.path;
  std::ifstream in(full_path.c_str(), std::ios::in | std::ios::binary);
  if(!in) {
    return common_response(Response::not_found);
  }

  Response response;

  response.response_code = Response::ok;
  response.content.assign(std::istreambuf_iterator<char>(in),
                       std::istreambuf_iterator<char>());

  response.headers["Content-Type"] = info.mime_type;

  return response;
}

}
