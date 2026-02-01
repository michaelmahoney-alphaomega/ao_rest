#include <string>
#include <fstream>
#include <vector>
#include <mutex>
#include <atomic>

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
    ERROR,
    WARNING,
    INFO,
    DEBUG
};

class Logger {
    private:
        ofstream LogFile;
        mutex logFileMutex;
        LogLevel log_level;
        atomic<int> debug_count {0};
        atomic<int> info_count {0};
        atomic<int> warning_count {0};
        atomic<int> error_count {0};
        vector<string> data {};

        void log(
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

        atomic<int>& log_error(string message);
        atomic<int>& log_warning(string message);
        atomic<int>& log_info(string message);
        atomic<int>& log_debug(string message);
        atomic<int>& get_error_count() const noexcept;
        atomic<int>& get_warning_count() const noexcept;
        atomic<int>& get_info_count() const noexcept;
        atomic<int>& get_debug_count() const noexcept;
        atomic<int>& flush();
        atomic<int>& roll_over();
        atomic<int>& roll_up(Logger logger);


};