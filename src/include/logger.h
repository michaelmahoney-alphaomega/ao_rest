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
    
    public:
        Logger(
            const string file_path,
            const int log_level = 2,
            const int rollover_size = 50000000,
            const bool send_to_cout = true,
            const vector<string>& supplied_data = {}
        );

        ~Logger();

        int& log_error(string message);
        int& log_warning(string message);
        int& log_info(string message);
        int& log_debug(string message);
        int& get_error_count() const noexcept;
        int& get_warning_count() const noexcept;
        int& get_info_count() const noexcept;
        int& get_debug_count() const noexcept;
        void flush();
        void roll_over();
        int roll_up(Logger logger);


};