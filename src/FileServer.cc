#include "FileServer.hh"

#include <fstream>

#include "CommonReplies.hh"
#include "URIInfo.hh"

namespace http {

int url_percent_decode(const std::string& in, std::string& out) {
  out.clear();
  out.reserve(in.size());

  for(std::size_t i = 0; i<in.size(); i++) {
    if(in[i] == '%') {
      if(i+3 <= in.size()) {
        int value = 0;
        std::istringstream is(in.substr(i+1, 2));
        if(is >> std::hex >> value) {
          out += static_cast<char>(value);
          i += 2;
        } else {
          return 1;
        }
      } else {
        return 1;
      }

    } else if (in[i] == '+') {
      out += ' ';

    } else {
      out += in[i];
    }
  }

  return 0;
}

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

  reply.headers.resize(2);

  reply.headers[0].name = "Content-Length";
  reply.headers[0].value = std::to_string(reply.content.size());
  reply.headers[1].name = "Content-Type";
  reply.headers[1].value = info.mime_type;

  return reply;
}

}
