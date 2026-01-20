#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

class Logger {
    ofstream LogFile;
    int debug_count = 0;
    int info_count = 0;
    int warning_count = 0;
    int error_count = 0;
    // vector<string> data;
    
    public:
        Logger(
            const string& file_path,
            const int log_level,
            const int rollover_size,
            const bool send_to_cout,
            vector<string> data = {}
            


        ){
            if (!LogFile.is_open()){

                time_t now = time(nullptr);
                tm* localTime = localtime(&now);
                char buffer[20];
                const size_t dateLength = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

                if (dateLength == 0) {
                    cerr << "ERROR: Logger.Logger() failed to create a formatted timestamp. Please check the Logger class constructor function." << endl;
                }

                const string errorMessage = buffer + string("ERROR: Logger.Logger() failed to open the log file: ") + file_path + string("Please make sure the file exists and this server has unique access to this log file while running");
                cerr << errorMessage << endl;
            }
            
            else {
                
                LogFile.open(file_path, ios::app);

            }
        }

        ~Logger(){
            if (LogFile.is_open()) {LogFile.close();}
        }
    
}