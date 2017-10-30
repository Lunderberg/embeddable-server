#include "FileServer.hh"

#include <fstream>

#include "CommonReplies.hh"
#include "URIInfo.hh"

namespace eweb {

FileServer::FileServer(std::string root_path)
  : root_path(root_path) { }

Reply FileServer::operator()(Request request) {
  URIInfo info(request.uri);

  if(!info.is_valid) {
    return common_reply(Reply::bad_request);
  }

  std::string full_path = root_path + info.path;
  std::ifstream in(full_path.c_str(), std::ios::in | std::ios::binary);
  if(!in) {
    return common_reply(Reply::not_found);
  }

  Reply reply;

  reply.response_code = Reply::ok;
  reply.content.assign(std::istreambuf_iterator<char>(in),
                       std::istreambuf_iterator<char>());

  reply.headers["Content-Type"] = info.mime_type;

  return reply;
}

}
