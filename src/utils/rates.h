#pragma once
#include "debug.h"
#include "request.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>

class Rates {
private:
    static int getWeekDayFromTimestamp(long timestamp)
    {
        struct tm* tm = localtime(&timestamp);
        return tm->tm_wday; // 0=Sunday, 5=Friday
    }

public:
    static std::vector<float> rates()
    {
        std::vector<float> rates;

        Request request("https://economia.awesomeapi.com.br/json/daily/USD-BRL/70");

        if (request.isSuccessful()) {
            JsonDocument doc;
            deserializeJson(doc, request.getResponse());

            for (JsonObject rate : doc.as<JsonArray>()) {
                float value = atof(rate["bid"]);
                rates.push_back(value);
            }
        }

        return rates;
    }

    static float currentRate()
    {
        debug("Getting current rate");
        Request request("https://economia.awesomeapi.com.br/json/last/USD-BRL");

        if (request.isSuccessful()) {
            debug("Current rate received");
            JsonDocument doc;
            debug("Deserializing current rate");
            deserializeJson(doc, request.getResponse());
            debug("Current rate deserialized");
            return atof(doc["USDBRL"]["bid"]);
        }

        return 0.0;
    }

    static float medianRate30DaysAgo()
    {
        debug("Getting median rate 30 days ago");
        Request request("https://economia.awesomeapi.com.br/json/daily/USD-BRL/30");

        if (request.isSuccessful()) {
            debug("Rate 30 days ago received");
            JsonDocument doc;
            debug("Deserializing rate 30 days ago");
            deserializeJson(doc, request.getResponse());
            debug("Rate 30 days ago deserialized");
            JsonArray arr = doc.as<JsonArray>();
            if (arr.size() > 0) {
                float sum = 0.0f;
                int count = 0;
                for (JsonObject rate : arr) {
                    sum += atof(rate["bid"]);
                    count++;
                }
                return sum / static_cast<float>(count);
            }
        }

        return 0.0;
    }
};