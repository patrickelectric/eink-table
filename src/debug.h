#pragma once

#define DEBUG

#ifdef DEBUG
#define debug(fmt, args ...)  do { \
        Serial.printf("[%s:%d]: " fmt "\n\r", __PRETTY_FUNCTION__, __LINE__, ## args); \
    } while(0)
#else
#define debug(fmt, args ...)
#endif