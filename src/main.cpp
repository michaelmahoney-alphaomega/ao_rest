#include <string>
#include <vector>
#include "http_server.hpp"

using namespace std;

int main(std::string api_call){
    string serverConfigFile = "server_config.json";
    vector<string> serverConfig = load_server_config(serverConfigFile);
    int error = http_server(api_call, serverConfig);
    return  error;
}