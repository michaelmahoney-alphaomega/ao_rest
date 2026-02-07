#include "logger.h"
#include <cassert>
using namespace std;

const string logFilePath = string("./main.log");
const LogLevel logLevel = debug_messages;
const int rolloverSize = 50000000;
const bool sendToCOut = true;
const vector<string> incommingData = {
    "123 ERROR: test test_func 0",
    "123 WARNING: test test_func 0",
    "123 WARNING: test test_func 0",
    "123 INFO: test test_func 0",
    "123 INFO: test test_func 0",
    "123 INFO: test test_func 0",
    "123 DEBUG: test test_func 0",
    "123 DEBUG: test test_func 0",
    "123 DEBUG: test test_func 0",
    "123 DEBUG: test test_func 0",
    "123 : test test_func 0",
};
const vector<string>& suppliedData = incommingData;


int main() {
    Logger Log = Logger(
        logFilePath,
        logLevel,
        rolloverSize,
        sendToCOut,
        suppliedData
    );
    
    cout << string("get_error_count TEST: Log.get_error_count() = ") << Log.get_error_count() << endl;
    cout << string("get_warning_count TEST: Log.get_warning_count() = ") << Log.get_warning_count() << endl;
    cout << string("get_info_count TEST: Log.get_info_count() = ") << Log.get_info_count() << endl;
    cout << string("get_debug_count TEST: Log.get_debug_count() = ") << Log.get_debug_count() << endl;
    int debugCount = Log.get_debug_count();
    int errorCount= Log.get_error_count();
    int warningCount = Log.get_warning_count();
    int infoCount = Log.get_info_count();

    assert(errorCount == 1);
    assert(warningCount == 2);
    assert(infoCount == 3);
    assert(debugCount == 4);


}