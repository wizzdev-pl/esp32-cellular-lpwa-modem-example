#ifndef D63F93CA_3220_48F0_B31D_F85061E24947
#define D63F93CA_3220_48F0_B31D_F85061E24947

#include "esp_modem_config.h"
#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_dte.hpp"

class LpwaCommunicationController
{
    constexpr static char *APN_NAME = "internet.gma.iot";
    constexpr static int GPIO_LEVEL_HIGH = 1;
    constexpr static int GPIO_LEVEL_LOW = 0;

public:
    void init();

private:
    void powerKeyGpioInit();
    void modulePowerOn();
    void modemNetworkConfiguration();

    esp_modem_dte_config_t m_dteConfig;
    std::shared_ptr<esp_modem::DTE> m_dte;
    esp_modem_dce_config_t m_dceConfig;
    std::unique_ptr<esp_modem::DCE> m_dce;
};

#endif /* D63F93CA_3220_48F0_B31D_F85061E24947 */
