// Please keep these 2 lines at the beginning of each cpp module - tag and local log level
static const char* LOG_TAG = "Main";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO


#include "defines.h"
#include "sleep.h"


void app_main(void)
{
    LOG_INFO("Hello from main!");

    while (1)
    {
        LOG_DEBUG("Hello loop");
        SLEEP_MS(1000);
    }
}

