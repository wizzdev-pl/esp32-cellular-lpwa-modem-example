static const char *LOG_TAG = "MQTT-Simple-Client";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "simple_mqtt_client_controller.h"

#include "defines.h"
#include "esp_event_base.h"
#include "sleep.h"

void _mqttEventHandler(void *handlerArgs, esp_event_base_t base, int32_t eventId, void *eventData)
{
    SimpleMqttClientController *pSimpleMqttClientController = static_cast<SimpleMqttClientController *>(eventData);
    pSimpleMqttClientController->mqttEventHandler(handlerArgs, base, eventId, eventData);
}

void SimpleMqttClientController::mqttEventHandler(void *handlerArgs, esp_event_base_t base, int32_t eventId, void *eventData)
{
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(eventData);

    switch (eventId)
    {
    case MQTT_EVENT_CONNECTED:
        LOG_INFO("MQTT_EVENT_CONNECTED");
        break;
    case MQTT_EVENT_DISCONNECTED:
        LOG_INFO("MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        LOG_INFO("MQTT_EVENT_SUBSCRIBED");
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        LOG_INFO("MQTT_EVENT_UNSUBSCRIBED");
        break;
    case MQTT_EVENT_PUBLISHED:
        LOG_INFO("MQTT_EVENT_PUBLISHED");
        break;
    case MQTT_EVENT_DATA:
        LOG_INFO("MQTT_EVENT_DATA");
        LOG_INFO("Message topic: %.*s", event->topic_len, event->topic);
        LOG_INFO("Message payload: %.*s", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        LOG_INFO("MQTT_EVENT_ERROR");
        break;
    default:
        LOG_INFO("MQTT - another event occurred with id: %ld", eventId);
    }
}

SimpleMqttClientController::SimpleMqttClientController()
{
    m_semaphoreReadyToSubscribe = xSemaphoreCreateBinary();
}

void SimpleMqttClientController::runTask()
{
    if (xTaskCreate(run, LOG_TAG, SIMPLE_MQTT_CONTROLLER_STACK_SIZE, this, DEFAULT_TASK_PRIORITY, &m_taskHandle) != pdPASS)
    {
        LOG_ERROR("Failed to create task: %s", LOG_TAG);
    }
}

void SimpleMqttClientController::run(void *pObject)
{
    SimpleMqttClientController *pSimpleMqttClientController = static_cast<SimpleMqttClientController *>(pObject);
    pSimpleMqttClientController->_run();
}

void SimpleMqttClientController::_run()
{
    init();
    perform();
}

void SimpleMqttClientController::setMqttClientReadyToSubscribe()
{
    xSemaphoreGive(m_semaphoreReadyToSubscribe);
}

void SimpleMqttClientController::init()
{
    esp_mqtt_client_config_t mqttConfig = {};

    mqttConfig.broker.address.uri = MQTT_BROKER_ADDRESS;
    mqttConfig.broker.address.port = MQTT_PORT;
    mqttConfig.broker.verification.skip_cert_common_name_check = true;

    mqttConfig.session.keepalive = MQTT_KEEPALIVE_VALUE;
    mqttConfig.buffer.size = MQTT_BUFFER_SIZE;

    mqttConfig.credentials.client_id = MQTT_CLIENT_ID;
    mqttConfig.credentials.username = MQTT_USERNAME;
    mqttConfig.credentials.authentication.password = MQTT_PASSWORD;

    mqttConfig.outbox.limit = MQTT_OUTBOX_LIMIT_BYTES;

    m_clientHandle = esp_mqtt_client_init(&mqttConfig);

    if (!m_clientHandle)
    {
        LOG_ERROR("Could not initialize mqttClient");
        return;
    }

    if (esp_mqtt_client_register_event(m_clientHandle, MQTT_EVENT_ANY, _mqttEventHandler, this) != ESP_OK)
    {
        LOG_ERROR("Could not register MQTT event in SimpleMqttClientController initialization");
        return;
    }
}

void SimpleMqttClientController::perform()
{
    if (esp_mqtt_client_start(m_clientHandle) != ESP_OK)
    {
        LOG_ERROR("Error while start MQTT Client");
    }

    LOG_INFO("Waiting for MQTT client to get connected");

    if (esp_mqtt_client_publish(m_clientHandle, "helloWorld", "helloFromSIM7080G!", 0, 1, 0) == MQTT_PUBLISH_ERROR)
    {
        LOG_ERROR("Error while sending initial message");
    }

    while (true)
    {
        SLEEP_MS(50);
    }
}
