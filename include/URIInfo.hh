#ifndef _URIINFO_H_
#define _URIINFO_H_

#include <map>
#include <string>

namespace eweb {

struct URIInfo {
  URIInfo(const std::string& request);

  bool is_valid;
  std::string path;
  std::string extension;
  std::string mime_type;
  std::map<std::string, std::string> get_params;

private:
  const char* parse_path(const char* begin, const char* end);
  void parse_extension();
  void generate_mime_type();
  void parse_get_params(const char* begin, const char* end);
};

}

#endif /* _URIINFO_H_ */
