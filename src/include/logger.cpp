#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <ctime>
#include <thread>
#include <chrono>

#include "logger.h"

using namespace std;
    
Logger::Logger(
    const string file_path,
    const int log_level,
    const int rollover_size,
    const bool sendToCOut,
    const vector<string>& supplied_data
){
    LogFile.open(file_path, ios::app);
    send_to_cout = sendToCOut;

    int failCounter = 0;
    while (!LogFile.is_open()){

        if (failCounter >= 5) {
            break;
        }

        LogFile.close();
        this_thread::sleep_for(chrono::seconds(1));
        LogFile.open(file_path, ios::app);
    }

    if (!LogFile.is_open()) {
        const string errorMessage = string("ERROR: Logger.Logger() failed to open the log file: ") + file_path + string(". Please make sure the file exists and this server has permissions to write to it.");
        cerr << errorMessage << endl;
    }

    else {
        for (const string& log_line : supplied_data) {
            const string errorLiteral = "ERROR";
            const string warningLiteral = "WARNING";
            const string infoLiteral = "INFO";
            const string debugLiteral = "DEBUG";

            if (log_line.find(errorLiteral) != string::npos) {
                error_count++; 
            }

            else if (log_line.find(warningLiteral ) != string::npos){
               warning_count++; 
            }

            else if (log_line.find(infoLiteral) != string::npos){
               info_count++;
            }

            else if (log_line.find(debugLiteral) != string::npos){
               debug_count++;
            }

            else {
                cerr << "There was an invalid line in the supplied data. Line = " << log_line <<endl;
                // do nothing if the string doesn't have any of the key words.
            }

            data.push_back(log_line);
        }
    }
}

Logger::~Logger(){
    if (LogFile.is_open()) {LogFile.close();}
}
    
void Logger::_log(
    string log_message, 
    LogMessageType messageType,
    const char* file_name,
    const char* function_name,
    const int line
) {
    time_t now= time(nullptr);
    tm* localTime = localtime(&now);
    char localTimeBuffer[32];
    const size_t dateLength = strftime(localTimeBuffer, sizeof(localTimeBuffer), "%Y-%m-%d %H:%M:%S", localTime);

    string logMessage;
    string messageTypeString;
    const string strLine = to_string(line);
    const string prefix = file_name + string(" ") + function_name + " " + strLine + " ";
    
    if (messageType == LogMessageType::ERROR){
        messageTypeString = string(" ERROR: ");
    }

    else if (messageType == LogMessageType::WARNING){
        messageTypeString = string(" WARNING: ");
    }

    else if (messageType == LogMessageType::INFO){
        messageTypeString = string(" INFO: ");
    }

    else if (messageType == LogMessageType::DEBUG) {
        messageTypeString = string(" DEBUG: ");
    }

    else if (messageType == LogMessageType::FATAL) {
        messageTypeString = string(" FATAL: ");
    }
    
    else {
        messageTypeString = string(" UNKNOWN: ");
    }

    logMessage = localTimeBuffer + messageTypeString + prefix + log_message;
    Logger::data.push_back(logMessage);
    if (Logger::send_to_cout)
    LogFile << logMessage << endl;
}

int Logger::get_error_count() const noexcept {
    return Logger::error_count.load();
}
int Logger::get_warning_count() const noexcept {
    return Logger::warning_count.load();
}
int Logger::get_info_count() const noexcept {
    return Logger::info_count.load();
}
int Logger::get_debug_count() const noexcept {
    return debug_count.load();
}

vector<string> Logger::get_topN_lines(int topN) {
    vector<string> dataSegment;
    if (topN <= -1) {
        dataSegment = data;
    }
    else {
        size_t dataLength = Logger::data.size();
        int maxIndex = dataLength - 1;
        if (topN > maxIndex) {
            topN = maxIndex;
        }

        int startingIndex = maxIndex - topN;

        for (int i = startingIndex; i <= maxIndex; i++) {
            dataSegment.push_back(data.at(i));
        }
    }
    return dataSegment;
}

int Logger::log_error(
    string message, 
    const char* file_name, 
    const char* function_name, 
    const int line
) {
    Logger::_log(message, LogMessageType::ERROR, file_name, function_name, line);
    Logger::error_count ++;
    return Logger::get_error_count();
}

int Logger::log_warning(
    string message, 
    const char* file_name, 
    const char* function_name, 
    const int line
) {
    Logger::_log(message, LogMessageType::WARNING, file_name, function_name, line);
    Logger::warning_count++;    
    return Logger::get_warning_count();
}

int Logger::log_info(
    string message, 
    const char* file_name, 
    const char* function_name, 
    const int line
) {
    Logger::_log(message, LogMessageType::INFO, file_name, function_name, line);
    Logger::info_count++;    
    return Logger::get_info_count();
}

int Logger::log_debug(
    string message, 
    const char* file_name, 
    const char* function_name, 
    const int line
) {
    Logger::_log(message, LogMessageType::DEBUG, file_name, function_name, line);
    Logger::debug_count++;    
    return Logger::get_debug_count();
}

void Logger::log_fatal(
    string message,
    const char *file_name,
    const char *function_name,
    const int line)
{
    Logger::_log(message, LogMessageType::FATAL, file_name, function_name, line);
}

void Logger::log_unknown(
    string message, 
    const char* file_name, 
    const char* function_name, 
    const int line
) {
    Logger::_log(message, LogMessageType::UNKNOWN, file_name, function_name, line);
}

int Logger::flush() {
    vector<string>& dataRef = data;
    int dataLength = dataRef.size();
    
    for (string& logLine : dataRef) {
        LogFile << logLine << "\n";
    }
    dataRef.clear();
    dataRef.shrink_to_fit();
    LogFile.flush();

    return dataLength;
}