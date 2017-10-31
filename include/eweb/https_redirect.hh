#pragma once

#include <string>
#include <sstream>

#include "CommonResponses.hh"
#include "Response.hh"
#include "Request.hh"

namespace eweb {

class https_redirect {
public:
  https_redirect(int https_port)
    : https_port(https_port) { }

  Response operator()(Request request) {
    Response response;

    response.response_code = Response::moved_temporarily;

    // If user didn't give the Host header, cajole them.
    if(request.headers.count("Host")==0) {
      return common_response(Response::bad_request);
    }

    // Pull out hostname, without port
    auto host = request.headers["Host"];
    host = host.substr(0, host.find_first_of(":"));

    // Add port
    std::stringstream ss;
    ss << "https://" << host << ":" << https_port;
    // Just in case request doesn't start with a slash, don't want to
    // result in a malformed url.
    if(request.uri.size()==0 || request.uri[0]!='/') {
      ss << "/";
    }
    ss << request.uri;

    response.headers["Location"] = ss.str();

    return response;
  }

private:
  int https_port;
};

}
