#pragma once

#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "debug.h"

class Request {
private:
    HTTPClient http;
    WiFiClientSecure client;
    bool success;
    int statusCode;
    String response;

public:
    Request(const String& url) : success(false), statusCode(0) {
        debug("HTTP Request to: %s", url.c_str());
        client.setInsecure();  // Allow insecure HTTPS connections
        client.setTimeout(10);
        http.begin(client, url);
        statusCode = http.GET();
        success = (statusCode == 200);

        if (success) {
            response = http.getString();
            debug("Request successful");
        } else {
            debug("Request failed with code: %d", statusCode);
        }
    }

    ~Request() {
        http.end();
        client.stop();
        debug("HTTP connection closed");
    }

    bool isSuccessful() const { return success; }
    int getStatusCode() const { return statusCode; }
    const String& getResponse() const { return response; }

    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
};
