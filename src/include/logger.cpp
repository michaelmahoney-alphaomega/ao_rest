#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>

#include "logger.h"

using namespace std;
    
Logger::Logger(
    const string file_path,
    const int log_level,
    const int rollover_size,
    const bool send_to_cout,
    const vector<string>& supplied_data
){
    if (!LogFile.is_open()){

        time_t now = time(nullptr);
        tm* localTime = localtime(&now);
        char buffer[25];
        const size_t dateLength = strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

        if (dateLength == 0) {
            cerr << "ERROR: Logger.Logger() failed to create a formatted timestamp. Please check the Logger class constructor function." << endl;
        }

        const string errorMessage = buffer + string("ERROR: Logger.Logger() failed to open the log file: ") + file_path + string("Please make sure the file exists and this server has unique access to this log file while running");
        cerr << errorMessage << endl;
    }
    
    else {
        for (const string& log_line : supplied_data) {
            const string errorLiteral = "ERROR";
            const string warningLiteral = "WARNING";
            const string infoLiteral = "INFO";
            const string debugLiteral = "DEBUG";

            if (log_line.find(errorLiteral)) {
               error_count++; 
            }
            else if (log_line.find(warningLiteral)){
               warning_count++; 
            }
            else if (log_line.find(infoLiteral)){
               info_count++;
            }
            else if (log_line.find(debugLiteral)){
               debug_count++;
            }
            else {
                // do nothing if the string doesn't have any of the key words.
            }
            
            data.push_back(log_line);
        }

        LogFile.open(file_path, ios::app);
    }
}

        ~Logger(){
            if (LogFile.is_open()) {LogFile.close();}
        }

        int& log_error() {
            return error_count;
        }
        int& log_warning() {
            return warning_count;
        }
        int& log_info() {
            return info_count;
        }
        int& log_debug() {
            return debug_count;
        }
        int& log_unknown() {
            return unknown_count;
        }
    
    private:
        void log(
            string log_message, 
            LogMessageType messageType,
            const char* file_name,
            const char* function_name,
            const int line
        ) {
            time_t now = time(nullptr);
            tm* localTime = localtime(&now);
            char localTimeBuffer[32];
            const size_t dateLength = strftime(localTimeBuffer, sizeof(localTimeBuffer), "%Y-%m-%d %H:%M:%S", localTime);

            if (dateLength == 0) {
                cerr << "ERROR: Logger.Logger() failed to create a formatted timestamp. Please check the Logger class constructor function." << endl;
            }
            
            else {
                if (messageType == LogMessageType::ERROR){
                    string strLine = to_string(line);
                    const string prefix = string(" ERROR: ") + file_name + " " + function_name + " " + strLine + " ";
                    const string logMessage = localTimeBuffer + prefix + log_message;
                    LogFile << logMessage << endl;
                }
            }

        }
    
};