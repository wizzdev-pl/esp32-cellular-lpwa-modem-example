static const char* LOG_TAG = "LPWA-Comm-Contr";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "lpwa_communication_controller.h"

#include "common/sleep.h"
#include "defines.h"
#include "hardware_definitions.h"

extern "C"
{
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_netif_defaults.h"
#include "esp_netif_ppp.h"
#include "esp_netif_types.h"
}

#include "cxx_include/esp_modem_api.hpp"
#include "cxx_include/esp_modem_command_library.hpp"

void _ipEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    LpwaCommunicationController* pLpwaCommunicationControlller = static_cast<LpwaCommunicationController*>(arg);
    pLpwaCommunicationControlller->ipEventCallback(arg, event_base, event_id, event_data);
}

void _pppStatusEventCallback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    LpwaCommunicationController* pLpwaCommunicationControlller = static_cast<LpwaCommunicationController*>(arg);
    pLpwaCommunicationControlller->pppStatusEventCallback(arg, event_base, event_id, event_data);
}

void LpwaCommunicationController::ipEventCallback(
    void*            arg,
    esp_event_base_t event_base,
    int32_t          event_id,
    void*            event_data)
{
    // LOG_INFO("Received IP Event: %u", event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP)
    {
        esp_netif_dns_info_t dnsInfo = {};

        ip_event_got_ip_t* event = reinterpret_cast<ip_event_got_ip_t*>(event_data);
        esp_netif_t*       netif = event->esp_netif;
        esp_netif_get_dns_info(netif, ESP_NETIF_DNS_MAIN, &dnsInfo);
        esp_netif_get_dns_info(netif, ESP_NETIF_DNS_BACKUP, &dnsInfo);

        LOG_INFO("Modem connection to PPP Server");
        LOG_INFO("-------------------------------");
        LOG_INFO("IP:            " IPSTR, IP2STR(&event->ip_info.ip));
        LOG_INFO("Netmask:       " IPSTR, IP2STR(&event->ip_info.netmask));
        LOG_INFO("Gateway:       " IPSTR, IP2STR(&event->ip_info.gw));
        LOG_INFO("Name Server 1: " IPSTR, IP2STR(&dnsInfo.ip.u_addr.ip4));
        LOG_INFO("Name Server 2: " IPSTR, IP2STR(&dnsInfo.ip.u_addr.ip4));
        LOG_INFO("-------------------------------");

        m_simpleMqttClientController.runTask();
    }
    else if (event_id == IP_EVENT_PPP_LOST_IP)
    {
        LOG_INFO("Modem disconnected from PPP Server");
    }
}

void LpwaCommunicationController::pppStatusEventCallback(
    void*            arg,
    esp_event_base_t event_base,
    int32_t          event_id,
    void*            event_data)
{
    LOG_INFO("NETIF_PPP_STATUS event callback");
}

void LpwaCommunicationController::runTask()
{
    if (xTaskCreate(run, LOG_TAG, DEFAULT_HUGE_STACK_SIZE, this, DEFAULT_TASK_PRIORITY, &m_taskHandle) != pdPASS)
    {
        LOG_ERROR("Failed to create task: %s", LOG_TAG);
    }
}

void LpwaCommunicationController::run(void* pObject)
{
    LpwaCommunicationController* pLpwaCommunicationController = static_cast<LpwaCommunicationController*>(pObject);
    pLpwaCommunicationController->_run();
}

void LpwaCommunicationController::_run()
{
    init();
    perform();
}

void LpwaCommunicationController::init()
{
    if (esp_netif_init() != ESP_OK)
    {
        LOG_ERROR("Could not initialize esp_netif");
    }

    // Init and register system/core components
    if (esp_event_loop_create_default() != ESP_OK)
    {
        LOG_ERROR("Error while creating esp_event loop");
    }

    if (esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, _ipEventCallback, this) != ESP_OK)
    {
        LOG_ERROR("Could not register event handler for IP_EVENT");
        return;
    }

    if (esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, _pppStatusEventCallback, this) != ESP_OK)
    {
        LOG_ERROR("Could not register event handler for NETIF_PPP_STATUS");
        return;
    }

    m_eventGroup = xEventGroupCreate();

    // initializing GPIO that's intented to wake up the module
    powerKeyGpioInit();

    // Configure the PPP netif
    esp_netif_config_t netif_ppp_config = ESP_NETIF_DEFAULT_PPP();

    // Create the PPP and DCE objects
    esp_netif_t* esp_netif = esp_netif_new(&netif_ppp_config);
    if (!esp_netif)
    {
        LOG_ERROR("Could not create esp_netif");
        return;
    }

    m_dteConfig                              = ESP_MODEM_DTE_DEFAULT_CONFIG();
    m_dteConfig.uart_config.tx_io_num        = hardware::LPWA_MODULE_UART_RXD_PIN;
    m_dteConfig.uart_config.rx_io_num        = hardware::LPWA_MODULE_UART_TXD_PIN;
    m_dteConfig.uart_config.port_num         = UART_NUM_1;
    m_dteConfig.uart_config.baud_rate        = LPWA_UART_BAUDRATE;
    m_dteConfig.uart_config.source_clk       = ESP_MODEM_DEFAULT_UART_CLK;
    m_dteConfig.uart_config.data_bits        = LPWA_UART_DATABITS;
    m_dteConfig.uart_config.stop_bits        = LPWA_UART_STOP_BITS;
    m_dteConfig.uart_config.parity           = LPWA_UART_PARITY;
    m_dteConfig.uart_config.flow_control     = LPWA_UART_FLOW_CONTROL;
    m_dteConfig.uart_config.event_queue_size = LPWA_UART_QUEUE_SIZE;
    m_dteConfig.uart_config.rx_buffer_size   = LPWA_UART_RX_BUFFER_SIZE;
    m_dteConfig.uart_config.tx_buffer_size   = LPWA_UART_TX_BUFFER_SIZE;

    m_dteConfig.task_stack_size = LPWA_DTE_STACK_SIZE;
    m_dteConfig.task_priority   = DEFAULT_TASK_PRIORITY;
    m_dteConfig.dte_buffer_size = LPWA_DTE_BUFFER_SIZE;

    m_dte = esp_modem::create_uart_dte(&m_dteConfig);
    if (!m_dte)
    {
        LOG_ERROR("Could not create DTE");
        return;
    }

    m_dceConfig = ESP_MODEM_DCE_DEFAULT_CONFIG(APN_NAME);

    // we are using driver for SIM7070, most of the commands are compatible with SIM7080
    m_dce = esp_modem::create_SIM7070_dce(&m_dceConfig, m_dte, esp_netif);
    if (!m_dce)
    {
        LOG_ERROR("Could not create DCE");
        return;
    }

    if (m_dce->sync() != esp_modem::command_result::OK)
    {
        LOG_WARNING("Modem not responding, setting command mode");
        m_dce->set_mode(esp_modem::modem_mode::COMMAND_MODE);

        if (m_dce->sync() != esp_modem::command_result::OK)
        {
            LOG_WARNING("Modem still not responding, starting power on sequence");
            modulePowerOn();
        }
    }

    // Network Configuration
    if (m_dce->set_baud(LPWA_UART_BAUDRATE) == esp_modem::command_result::OK)
    {
        LOG_INFO("Baud rate set correctly");
    }
    else
    {
        LOG_ERROR("Could not set baudrate");
    }

    if (m_dce->set_network_bands(
            std::string(LTE_M_STRING), LTE_M_BANDS, sizeof(LTE_M_BANDS) / sizeof(LTE_M_BANDS[0])) ==
        esp_modem::command_result::OK)
    {
        LOG_INFO("Network bands for LTE-M set successfully");
    }
    else
    {
        LOG_ERROR("Error while attempting to set network bands for LTE-M");
    }

    if (m_dce->set_network_bands(
            std::string(NB_IOT_STRING), NB_IOT_BANDS, sizeof(NB_IOT_BANDS) / sizeof(NB_IOT_BANDS[0])) ==
        esp_modem::command_result::OK)
    {
        LOG_INFO("Network bands for NB-IoT set successfully");
    }
    else
    {
        LOG_ERROR("Error while attempting to set NB-IoT Bands");
    }

    if (m_dce->set_preferred_mode(PREFERRED_NETWORK_MODE) == esp_modem::command_result::OK)
    {
        LOG_INFO("Preferred mode set correctly");
    }
    else
    {
        LOG_ERROR("Error while attempting to set preferred mode");
    }

    // Checking signal
    int bitErrorRate = 0;
    int rssiValue    = 0;

    if (m_dce->get_signal_quality(rssiValue, bitErrorRate) == esp_modem::command_result::OK)
    {
        LOG_INFO("Received response to signal quality check, RSSI: %d, BER: %d", rssiValue, bitErrorRate);
    }

    if ((rssiValue == LPWA_NO_SIGNAL_VALUE) || (rssiValue < LPWA_WEAK_SIGNAL_VALUE))
    {
        LOG_ERROR("Weak signal, stopping configuration");
        return;
    }

    // entering data mode
    LOG_INFO("Entering data mode now");

    if (!(m_dce->set_mode(esp_modem::modem_mode::DATA_MODE)))
    {
        LOG_ERROR("Setting data mode failed");
        return;
    }
}

void LpwaCommunicationController::perform()
{
    while (true)
    {
        SLEEP_MS(50);
    }
}

void LpwaCommunicationController::powerKeyGpioInit()
{
    gpio_config_t powerKeyGpioConfig;

    powerKeyGpioConfig.intr_type    = static_cast<gpio_int_type_t>(GPIO_INTR_DISABLE);
    powerKeyGpioConfig.mode         = GPIO_MODE_OUTPUT;
    powerKeyGpioConfig.pin_bit_mask = (1ULL << hardware::LPWA_MODULE_POWER_KEY);
    powerKeyGpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    powerKeyGpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpio_config(&powerKeyGpioConfig);
}

void LpwaCommunicationController::modulePowerOn()
{
    LOG_INFO("Starting SIM7080G powerOn");

    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_LOW);
    SLEEP_MS(1000);
    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_HIGH);
    SLEEP_MS(2000);
    gpio_set_level(hardware::LPWA_MODULE_POWER_KEY, GPIO_LEVEL_LOW);
    SLEEP_MS(5000);

    LOG_INFO("Power on procedure finished");
}