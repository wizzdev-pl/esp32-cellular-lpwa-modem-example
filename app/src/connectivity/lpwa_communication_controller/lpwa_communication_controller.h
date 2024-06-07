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
    constexpr static char APN_NAME[] = "internet.gma.iot";
    constexpr static int GPIO_LEVEL_HIGH = 1;
    constexpr static int GPIO_LEVEL_LOW = 0;

    constexpr static int LPWA_UART_BAUDRATE = 115200;
    constexpr static uart_word_length_t LPWA_UART_DATABITS = UART_DATA_8_BITS;
    constexpr static uart_stop_bits_t LPWA_UART_STOP_BITS = UART_STOP_BITS_1;
    constexpr static uart_parity_t LPWA_UART_PARITY = UART_PARITY_DISABLE;
    constexpr static esp_modem_flow_ctrl_t LPWA_UART_FLOW_CONTROL = ESP_MODEM_FLOW_CONTROL_NONE;
    constexpr static int LPWA_UART_QUEUE_SIZE = 30;
    constexpr static int LPWA_UART_RX_BUFFER_SIZE = 4096;
    constexpr static int LPWA_UART_TX_BUFFER_SIZE = 512;

    constexpr static int LPWA_DTE_STACK_SIZE = 6144;
    constexpr static int LPWA_DTE_BUFFER_SIZE = 512;

    constexpr static int LPWA_NO_SIGNAL_VALUE = 99;
    constexpr static int LPWA_WEAK_SIGNAL_VALUE = 9;

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
    void perform();

    TaskHandle_t m_taskHandle;
    esp_modem_dte_config_t m_dteConfig;
    std::shared_ptr<esp_modem::DTE> m_dte;
    esp_modem_dce_config_t m_dceConfig;
    std::unique_ptr<esp_modem::DCE> m_dce;
    EventGroupHandle_t m_eventGroup;
};

#endif /* D63F93CA_3220_48F0_B31D_F85061E24947 */
