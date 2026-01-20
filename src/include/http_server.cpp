#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include "http_server.hpp"

using namespace std;

vector<string> load_server_config(const string& server_config_path) {
    vector<string> configReturn;
    ifstream ConfigFile(server_config_path);
    
    if (!ConfigFile.is_open()) {
        configReturn.push_back("ERROR: Failed to open configuration file.");
        return configReturn;
    }
    


}

int http_server(string api_string, vector<string> server_config)
{
    return 1;
}