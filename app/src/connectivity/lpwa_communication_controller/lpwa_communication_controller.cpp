static const char *LOG_TAG = "LPWA-Comm-Contr";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "lpwa_communication_controller.h"

#include "defines.h"
#include "hardware_definitions.h"
#include "common/sleep.h"

#include "esp_netif.h"
#include "esp_netif_defaults.h"
#include "esp_log.h"
#include "esp_event.h"

#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_command_library.hpp"
#include "driver/gpio.h"

void LpwaCommunicationController::init()
{
    esp_netif_init();

    /* Init and register system/core components */
    esp_event_loop_create_default();

    powerKeyGpioInit();

    /* Configure the PPP netif */
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();

    /* Create the PPP and DCE objects */

    esp_netif_t *esp_netif = esp_netif_new(&netif_ppp_config);
    if (!esp_netif)
    {
        LOG_ERROR("Could not create esp_netif");
        return;
    }

    m_dteConfig = ESP_MODEM_DTE_DEFAULT_CONFIG();
    m_dteConfig.uart_config.tx_io_num = hardware::LPWA_MODULE_UART_RXD_PIN;
    m_dteConfig.uart_config.rx_io_num = hardware::LPWA_MODULE_UART_TXD_PIN;
    m_dteConfig.uart_config.port_num = UART_NUM_1;
    m_dteConfig.uart_config.baud_rate = 115200;
    m_dteConfig.uart_config.source_clk = ESP_MODEM_DEFAULT_UART_CLK;
    m_dteConfig.uart_config.data_bits = UART_DATA_8_BITS;
    m_dteConfig.uart_config.stop_bits = UART_STOP_BITS_1;
    m_dteConfig.uart_config.parity = UART_PARITY_DISABLE;
    m_dteConfig.uart_config.flow_control = ESP_MODEM_FLOW_CONTROL_NONE;
    m_dteConfig.uart_config.event_queue_size = 30;
    m_dteConfig.uart_config.rx_buffer_size = 4096;
    m_dteConfig.uart_config.tx_buffer_size = 512;

    m_dteConfig.task_stack_size = 6144;
    m_dteConfig.task_priority = 5;
    m_dteConfig.dte_buffer_size = 512;

    m_dte = esp_modem::create_uart_dte(&m_dteConfig);
    if (!m_dte)
    {
        LOG_ERROR("Could not create DTE");
        return;
    }

    m_dceConfig = ESP_MODEM_DCE_DEFAULT_CONFIG(APN_NAME);

    m_dce = esp_modem::create_SIM7070_dce(&m_dceConfig, m_dte, esp_netif);
    if (!m_dce)
    {
        LOG_ERROR("Could not create DCE");
        return;
    }

    modulePowerOn();
    modemNetworkConfiguration();
}

void LpwaCommunicationController::powerKeyGpioInit()
{
    gpio_config_t powerKeyGpioConfig;

    powerKeyGpioConfig.intr_type = static_cast<gpio_int_type_t>(GPIO_INTR_DISABLE);
    powerKeyGpioConfig.mode = GPIO_MODE_OUTPUT;
    powerKeyGpioConfig.pin_bit_mask = (1ULL << hardware::LPWA_MODULE_POWER_KEY);
    powerKeyGpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    powerKeyGpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&powerKeyGpioConfig);
}

void LpwaCommunicationController::modulePowerOn()
{
    LOG_INFO("Starting SIM7080G powerOn");

    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_HIGH);
    SLEEP_MS(1000);
    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_HIGH);
    SLEEP_MS(2000);
    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_LOW);
    SLEEP_MS(5000);

    LOG_INFO("Power on procedure finished");
}

void LpwaCommunicationController::modemNetworkConfiguration()
{
    if (m_dce == nullptr)
    {
        LOG_ERROR("DCE not initialized");
        return;
    }

    // set baudrate

    if (m_dce->set_baud(115200) == esp_modem::command_result::OK)
    {
        LOG_INFO("Baud rate set correctly");
    }
    else
    {
        LOG_ERROR("Could not set baudrate");
    }
}