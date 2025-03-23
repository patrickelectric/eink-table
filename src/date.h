#pragma once

#include <Arduino.h>
#include <time.h>

class Date {
public:
    static void init() {
        const char* ntpServer = "pool.ntp.org";
        const long gmtOffset_sec = -10800; // GMT-3 for Brazil // 3 hours * 3600 seconds/hour
        const int daylightOffset_sec = 0;
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    }

    static tm today() {
        struct tm timeinfo;
        if (!getLocalTime(&timeinfo)) {
            Serial.println("Failed to get time");
            memset(&timeinfo, 0, sizeof(timeinfo));
        }
        return timeinfo;
    }
};

bool operator==(const tm& a, const tm& b) {
  return a.tm_year == b.tm_year &&
         a.tm_mon  == b.tm_mon  &&
         a.tm_mday == b.tm_mday;
}