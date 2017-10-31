#pragma once

namespace eweb {

Response show_params(Request request) {
  Response output;

  std::stringstream ss;
  ss << "<html><body>";
  ss << "Page requested: " << request.uri << "<br>";

  ss << "<table border=1><tr><th colspan=2>GET Params</th></tr>";
  ss << "<tr><th>Key</th><th>Value</th></tr>";

  URIInfo uri_info(request.uri);
  for(const auto& entry : uri_info.get_params) {
    ss << "<tr><td>" << entry.first << "</td><td>"
       << entry.second << "</td></tr>";
  }

  ss << "</table>";
  ss << "</body></html>";

  output.response_code = Response::ok;
  output.content = ss.str();

  return output;
}

}
