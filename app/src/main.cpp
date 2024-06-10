// Please keep these 2 lines at the beginning of each cpp module - tag and local log level
static const char *LOG_TAG = "Main";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "defines.h"
#include "sleep.h"

#include "lpwa_communication_controller.h"

void run(void);

extern "C"
{
    void app_main(void)
    {
        run();
    }
}

void run(void)
{
    LOG_INFO("Hello from main!");

    LpwaCommunicationController lpwaCommunicationController;

    lpwaCommunicationController.runTask();

    while (1)
    {
        LOG_DEBUG("Hello loop");
        SLEEP_MS(1000);
    }
}
