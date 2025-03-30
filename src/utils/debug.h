#pragma once

#include <LittleFS.h>
#include <vector>

class Logger {
private:
    static Logger* instance;
    String currentLogPath;
    File logFile;
    bool initialized;

    Logger() : initialized(false) {
        if (!LittleFS.begin(true)) {
            return;
        }
        int nextIndex = 0;
        File root = LittleFS.open("/");
        File file = root.openNextFile();
        while(file) {
            String fname = file.name();
            Serial.println("Looking at " + fname);
            if (fname.startsWith("log_") && fname.endsWith(".txt")) {
                int idx = fname.substring(4, fname.length()-4).toInt();
                if (idx >= nextIndex) nextIndex = idx + 1;
            }
            file = root.openNextFile();
        }

        currentLogPath = "/log_" + String(nextIndex) + ".txt";
        Serial.println("Opening " + currentLogPath);
        logFile = LittleFS.open(currentLogPath, "a");
        if (logFile) {
            initialized = true;
        }
    }

public:
    static Logger* getInstance() {
        if (instance == nullptr) {
            instance = new Logger();
        }
        return instance;
    }

    void log(const char* message) {
        if (initialized && logFile) {
            logFile.print(message);
            logFile.flush();
        } else {
            Serial.println("Failed to open log file");
        }
    }

    String getFullLog() {
        String result = "";
        if (!initialized) {
            return "Logger not initialized";
        }

        logFile.close();

        File readFile = LittleFS.open(currentLogPath, "r");
        if (!readFile) {
            logFile = LittleFS.open(currentLogPath, "a");
            return "Failed to read log file";
        }

        while (readFile.available()) {
            result += (char)readFile.read();
        }

        readFile.close();
        logFile = LittleFS.open(currentLogPath, "a");

        return result;
    }

    std::vector<String> listLogFiles() {
        std::vector<String> logFiles;
        if (!initialized) {
            return logFiles;
        }

        File root = LittleFS.open("/");
        File file = root.openNextFile();

        while(file) {
            String fname = file.name();
            if (fname.startsWith("log_") && fname.endsWith(".txt")) {
                logFiles.push_back("/" + fname);
            }
            file = root.openNextFile();
        }

        // Sort files by index number
        std::sort(logFiles.begin(), logFiles.end(), [](const String& a, const String& b) {
            int idxA = a.substring(4, a.length()-4).toInt();
            int idxB = b.substring(4, b.length()-4).toInt();
            return idxA < idxB;
        });

        return logFiles;
    }

    String readLogFile(const String& filename) {
        String result = "";
        if (!initialized) {
            return "Logger not initialized";
        }

        // Check if we're trying to read the current log file
        if (filename == currentLogPath) {
            return getFullLog();
        }

        File readFile = LittleFS.open(filename, "r");
        if (!readFile) {
            return "Failed to open file: " + filename;
        }

        while (readFile.available()) {
            result += (char)readFile.read();
        }

        readFile.close();
        return result;
    }

    ~Logger() {
        if (logFile) {
            logFile.close();
        }
        LittleFS.end();
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};

Logger* Logger::instance = nullptr;

#define DEBUG

#ifdef DEBUG
#define debug(fmt, args...)                                                      \
    do {                                                                         \
        char buffer[256];                                                        \
        snprintf(buffer, sizeof(buffer), "[%s:%d]: " fmt "\n\r",                 \
            __PRETTY_FUNCTION__, __LINE__, ##args);                              \
        Serial.print(buffer);                                                    \
        snprintf(buffer, sizeof(buffer), "[%s:%d]: " fmt "\n",                   \
            __PRETTY_FUNCTION__, __LINE__, ##args);                              \
        Logger::getInstance()->log(buffer);                                      \
    } while (0)
#else
#define debug(fmt, args...)
#endif