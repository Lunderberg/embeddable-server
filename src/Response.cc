#include "Response.hh"

namespace eweb {

static asio::const_buffer response_code_to_buf(Response::ResponseCode response_code) {
  static const std::string ok = "HTTP/1.0 200 OK\r\n";
  static const std::string created = "HTTP/1.0 201 Created\r\n";
  static const std::string accepted = "HTTP/1.0 202 Accepted\r\n";
  static const std::string no_content = "HTTP/1.0 204 No Content\r\n";
  static const std::string multiple_choices = "HTTP/1.0 300 Multiple Choices\r\n";
  static const std::string moved_permanently = "HTTP/1.0 301 Moved Permanently\r\n";
  static const std::string moved_temporarily = "HTTP/1.0 302 Moved Temporarily\r\n";
  static const std::string not_modified = "HTTP/1.0 304 Not Modified\r\n";
  static const std::string bad_request = "HTTP/1.0 400 Bad Request\r\n";
  static const std::string unauthorized = "HTTP/1.0 401 Unauthorized\r\n";
  static const std::string forbidden = "HTTP/1.0 403 Forbidden\r\n";
  static const std::string not_found = "HTTP/1.0 404 Not Found\r\n";
  static const std::string internal_server_error = "HTTP/1.0 500 Internal Server Error\r\n";
  static const std::string not_implemented = "HTTP/1.0 501 Not Implemented\r\n";
  static const std::string bad_gateway = "HTTP/1.0 502 Bad Gateway\r\n";
  static const std::string service_unavailable = "HTTP/1.0 503 Service Unavailable\r\n";

  switch(response_code) {
    case Response::ok:
      return asio::buffer(ok);
    case Response::created:
      return asio::buffer(created);
    case Response::accepted:
      return asio::buffer(accepted);
    case Response::no_content:
      return asio::buffer(no_content);
    case Response::multiple_choices:
      return asio::buffer(multiple_choices);
    case Response::moved_permanently:
      return asio::buffer(moved_permanently);
    case Response::moved_temporarily:
      return asio::buffer(moved_temporarily);
    case Response::not_modified:
      return asio::buffer(not_modified);
    case Response::bad_request:
      return asio::buffer(bad_request);
    case Response::unauthorized:
      return asio::buffer(unauthorized);
    case Response::forbidden:
      return asio::buffer(forbidden);
    case Response::not_found:
      return asio::buffer(not_found);
    case Response::internal_server_error:
      return asio::buffer(internal_server_error);
    case Response::not_implemented:
      return asio::buffer(not_implemented);
    case Response::bad_gateway:
      return asio::buffer(bad_gateway);
    case Response::service_unavailable:
      return asio::buffer(service_unavailable);

    default:
      return asio::buffer(internal_server_error);
  }
}

static asio::const_buffer header_sep() {
  static const std::string val = ": ";
  return asio::buffer(val);
}

static asio::const_buffer endline() {
  static const std::string val = "\r\n";
  return asio::buffer(val);
}

std::vector<asio::const_buffer> Response::asio_buffers() {
  std::vector<asio::const_buffer> bufs;

  bufs.push_back(response_code_to_buf(response_code));

  for(const auto& head : headers) {
    bufs.push_back(asio::buffer(head.first));
    bufs.push_back(header_sep());
    bufs.push_back(asio::buffer(head.second));
    bufs.push_back(endline());
  }
  bufs.push_back(endline());
  bufs.push_back(asio::buffer(content));

  return bufs;
}

}
