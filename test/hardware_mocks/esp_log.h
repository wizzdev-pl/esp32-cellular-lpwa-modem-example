#ifndef __ESP_LOG_H__
#define __ESP_LOG_H__

#include <stdio.h>


typedef enum {
    ESP_LOG_NONE = 0,       /*!< No log output */
    ESP_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    ESP_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    ESP_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    ESP_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    ESP_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} esp_log_level_t;


#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL  ESP_LOG_INFO
#endif


static const char* logLevelToString(esp_log_level_t level)
{
    switch (level)
    {
        case ESP_LOG_ERROR:
            return "ERROR";
            break;
        case ESP_LOG_WARN:
            return "WARN ";
            break;
        case ESP_LOG_INFO:
            return "INFO ";
            break;
        case ESP_LOG_DEBUG:
            return "DEBUG";
            break;
        case ESP_LOG_VERBOSE:
            return "VERBO";
            break;

    }
}

#define ESP_LOG_LEVEL_LOCAL(level, tag, ...) do {               \
    if ( LOG_LOCAL_LEVEL >= level ) { printf("### [%s] %s: \t", logLevelToString(level), tag); printf(__VA_ARGS__); printf("\n"); } \
} while(0)


#define ESP_LOGE(tag, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, tag, __VA_ARGS__)
#define ESP_LOGW(tag, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN, tag, __VA_ARGS__)
#define ESP_LOGI(tag, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, tag, __VA_ARGS__)
#define ESP_LOGD(tag, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG, tag, __VA_ARGS__)
#define ESP_LOGV(tag, ...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, tag, __VA_ARGS__)

#endif  // __ESP_LOG_H__
