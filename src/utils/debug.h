#pragma once

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
    } while (0)
#else
#define debug(fmt, args...)
#endif