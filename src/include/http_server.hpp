#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <string>

int http_server(std::string api_string, std::vector<std::string> server_config);
std::vector<std::string> load_server_config(std::string server_config_file);

#endif