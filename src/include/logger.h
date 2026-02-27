#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <mutex>
#include <atomic>
#include <iostream>
#include <syncstream>
#include <ctime>
#include <thread>
#include <chrono>

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
        string log_file_path;
        string log_archive_folder;
        LogLevel log_level;
        uintmax_t log_file_rollover_size;
        mutex data_mutex;
        mutex log_file_mutex;
        mutex cout_mutex;
        mutex cerr_mutex;
        bool send_to_cout;
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
            const string log_archive_path,
            const int log_level = 2,
            const uintmax_t = 50000000,
            const bool send_to_cout = true,
            const vector<string>& supplied_data = {}
        );
        ~Logger();
        
        const string get_local_time();
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
        void rollover();
};