#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <vector>
#include "debug.h"
class Rates {
private:
    static int getWeekDayFromTimestamp(long timestamp) {
        struct tm *tm = localtime(&timestamp);
        return tm->tm_wday; // 0=Sunday, 5=Friday
    }

public:
    static std::vector<float> last10FridaysRates() {
        std::vector<float> fridaysRates;

        HTTPClient http;
        http.begin("https://economia.awesomeapi.com.br/json/daily/USD-BRL/70");
        int code = http.GET();

        if (code == 200) {
            String payload = http.getString();
            JsonDocument doc;
            deserializeJson(doc, payload);

            for (JsonObject rate : doc.as<JsonArray>()) {
            long timestamp = atol(rate["timestamp"]);
            if (getWeekDayFromTimestamp(timestamp) == 5) { // Friday
                float value = atof(rate["bid"]);
                fridaysRates.push_back(value);
            }

            if (fridaysRates.size() >= 10) break;
            }
        }
        http.end();

        return fridaysRates;
    }

    static float currentRate() {
        float currentRate = 0.0;

        HTTPClient http;
        debug("Getting current rate");
        http.begin("https://economia.awesomeapi.com.br/json/last/USD-BRL");
        int code = http.GET();

        if (code == 200) {
            debug("Current rate received");
            String payload = http.getString();
            JsonDocument doc;
            debug("Deserializing current rate");
            deserializeJson(doc, payload);
            debug("Current rate deserialized");
            currentRate = atof(doc["USDBRL"]["bid"]);
        }

        debug("Ending current rate");
        http.end();
        return currentRate;
    }

    static float rate30DaysAgo() {
        float rate = 0.0;

        HTTPClient http;
        http.begin("https://economia.awesomeapi.com.br/json/daily/USD-BRL/30");
        int code = http.GET();

        if (code == 200) {
            String payload = http.getString();
            JsonDocument doc;
            deserializeJson(doc, payload);

            JsonArray arr = doc.as<JsonArray>();
            if (arr.size() > 0) {
                JsonObject oldest = arr[arr.size() - 1];
                rate = atof(oldest["bid"]);
            }
        }
        http.end();
        return rate;
    }
};