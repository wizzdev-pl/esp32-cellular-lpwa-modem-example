#ifndef D63F93CA_3220_48F0_B31D_F85061E24947
#define D63F93CA_3220_48F0_B31D_F85061E24947

extern "C"
{
#include "esp_modem_config.h"
}

#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dte.hpp"

class LpwaCommunicationController
{
    constexpr static char *APN_NAME = "internet.gma.iot";
    constexpr static int GPIO_LEVEL_HIGH = 1;
    constexpr static int GPIO_LEVEL_LOW = 0;

public:
    void runTask();
    static void run(void *pObject);
    void _run();

    void init();
    void ipEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    void pppStatusEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

private:
    void powerKeyGpioInit();
    void modulePowerOn();
    void modemNetworkConfiguration();
    void perform();

    TaskHandle_t m_taskHandle;
    esp_modem_dte_config_t m_dteConfig;
    std::shared_ptr<esp_modem::DTE> m_dte;
    esp_modem_dce_config_t m_dceConfig;
    std::unique_ptr<esp_modem::DCE> m_dce;
    EventGroupHandle_t m_eventGroup;
};

#endif /* D63F93CA_3220_48F0_B31D_F85061E24947 */
