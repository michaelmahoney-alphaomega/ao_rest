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

            else if (log_line.find(warningLiteral) != string::npos){
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
    string localTimeString = string(localTimeBuffer);

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

    logMessage = localTimeString + messageTypeString + prefix + log_message;
    lock_guard(data_mutex);
    Logger::data.push_back(logMessage);
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
    
    lock_guard(log_file_mutex);
    for (string& logLine : dataRef) {
        LogFile << logLine << "\n";
    }

    lock_guard(data_mutex);
    dataRef.clear();
    dataRef.shrink_to_fit();
    LogFile.flush();

    return dataLength;
}

const string Logger::get_local_time() {
    time_t now= time(nullptr);
    tm* localTime = localtime(&now);
    char localTimeBuffer[32];
    const size_t dateLength = strftime(localTimeBuffer, sizeof(localTimeBuffer), "%Y-%m-%d %H:%M:%S", localTime);
    const string localTimeString = string(localTimeBuffer);
    return localTimeString;
}


void Logger::rollover() {
    filesystem::path logFilePath(Logger::log_file_path);
    string timeStamp = Logger::get_local_time();
    string logArchiveName = Logger::log_archive_folder + Logger::log_file_path + timeStamp;
    filesystem::path logFileArchivePath(logArchiveName);

    if (!filesystem::exists(logFilePath) || !filesystem::is_regular_file(logFilePath)) {
        lock_guard(cerr_mutex);
        cerr << Logger::get_local_time() + string(" ERROR: Logger::rollover() tried to read the size of -- ") + Logger::log_file_path + " -- and failed. This means the file either does not exist or is not a normal file type." << endl;
    }
    else {
        uintmax_t logFileSize = filesystem::file_size(logFilePath);
        if (logFileSize > Logger::log_file_rollover_size) {
            
            if (!filesystem::exists(logFileArchivePath) || !filesystem::is_directory(logFileArchivePath)) {
                lock_guard(cerr_mutex);
                cerr << Logger::get_local_time() + string(" ERROR: Logger::rollover() failed to copy the current log: ") + Logger::log_file_path + " to the archive folder: " + Logger::log_archive_folder + ", this means either the archive folder doesn't exist or isn't a directory." << endl;
                
                lock_guard(log_file_mutex);
                Logger::LogFile << Logger::get_local_time() + string(" ERROR: Logger::rollover() failed to copy the current log: ") + Logger::log_file_path + " to the archive folder: " + Logger::log_archive_folder + ", this means either the archive folder doesn't exist or isn't a directory." << endl;
                
            }

            bool copySuccess = filesystem::copy_file(logFilePath, logFileArchivePath);
            
            if (!copySuccess) {
                lock_guard(cerr_mutex);
                cerr << Logger::get_local_time() + string(" ERROR: Logger::rollover() failed to copy the currect log at ") + Logger::log_file_path + " to the archive path " + Logger::log_archive_folder + ". in this case the copy operation itself failed. Check that the file isn't locked by another process/application." << endl;

                lock_guard(log_file_mutex);
                LogFile << Logger::get_local_time() + string(" ERROR: Logger::rollover() failed to copy the currect log at ") + Logger::log_file_path + " to the archive path " + Logger::log_archive_folder + ". in this case the copy operation itself failed. Check that the file isn't locked by another process/application." << endl;
            }

        }
    }
}