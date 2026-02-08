#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>

using namespace std;

enum LogMessageType {
    FATAL,
    ERROR,
    WARNING,
    INFO,
    DEBUG,
    UNKNOWN
};

enum LogLevel {
    error_messages,
    warning_messages,
    info_messages,
    debug_messages
};

class Logger {
    private:
        ofstream LogFile;
        string log_file_name;
        mutex logFileMutex;
        LogLevel log_level;
        atomic<int> debug_count {0};
        atomic<int> info_count {0};
        atomic<int> warning_count {0};
        atomic<int> error_count {0};
        vector<string> data {};

        void _log(
            string log_message, 
            LogMessageType messageType,
            const char* file_name,
            const char* function_name,
            const int line
        );
        
    public:

        Logger(
            const string file_path,
            const int log_level = 2,
            const int rollover_size = 50000000,
            const bool send_to_cout = true,
            const vector<string>& supplied_data = {}
        );
        ~Logger();
        
        vector<string> get_topN_lines(int index);
        void log_fatal(string message, const char* file_name, const char* function_name, const int line);
        void log_unknown(string message, const char* file_name, const char* function_name, const int line);
        int log_error(string message, const char* file_name, const char* function_name, const int line);
        int log_warning(string message, const char* file_name, const char* function_name, const int line);
        int log_info(string message, const char* file_name, const char* function_name, const int line);
        int log_debug(string message, const char* file_name, const char* function_name, const int line);
        int get_error_count() const noexcept;
        int get_warning_count() const noexcept;
        int get_info_count() const noexcept;
        int get_debug_count() const noexcept;
        int flush();
        atomic<int>& roll_over();
        atomic<int>& roll_up(Logger logger);


};