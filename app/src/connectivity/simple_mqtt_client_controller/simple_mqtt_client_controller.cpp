static const char* LOG_TAG = "MQTT-Simple-Client";
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "simple_mqtt_client_controller.h"

#include "aws_configuration.h"
#include "defines.h"
#include "esp_event_base.h"
#include "sleep.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>

void _mqttEventHandler(void* handlerArgs, esp_event_base_t base, int32_t eventId, void* eventData)
{
    SimpleMqttClientController* pSimpleMqttClientController = static_cast<SimpleMqttClientController*>(handlerArgs);
    pSimpleMqttClientController->mqttEventHandler(handlerArgs, base, eventId, eventData);
}

void SimpleMqttClientController::mqttEventHandler(
    void*            handlerArgs,
    esp_event_base_t base,
    int32_t          eventId,
    void*            eventData)
{
    esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(eventData);

    switch (eventId)
    {
        case MQTT_EVENT_CONNECTED:
            LOG_INFO("MQTT_EVENT_CONNECTED");
            xSemaphoreGive(m_semaphoreBrokerConnected);
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

SimpleMqttClientController::SimpleMqttClientController() : m_semaphoreBrokerConnected(xSemaphoreCreateBinary())
{
}

void SimpleMqttClientController::runTask()
{
    if (xTaskCreate(run, LOG_TAG, SIMPLE_MQTT_CONTROLLER_STACK_SIZE, this, DEFAULT_TASK_PRIORITY, &m_taskHandle) !=
        pdPASS)
    {
        LOG_ERROR("Failed to create task: %s", LOG_TAG);
    }
}

void SimpleMqttClientController::run(void* pObject)
{
    SimpleMqttClientController* pSimpleMqttClientController = static_cast<SimpleMqttClientController*>(pObject);
    pSimpleMqttClientController->_run();
}

void SimpleMqttClientController::_run()
{
    init();
    perform();
}

void SimpleMqttClientController::init()
{
    // intializing the seed with current time so that same series of random values will not be drawn every time
    srand(time(NULL));

    esp_mqtt_client_config_t mqttConfig = {};

    mqttConfig.broker.address.uri                              = AWS_ENDPOINT_URL;
    mqttConfig.broker.address.port                             = MQTT_PORT;
    mqttConfig.broker.verification.skip_cert_common_name_check = true;

    mqttConfig.session.keepalive = MQTT_KEEPALIVE_VALUE;
    mqttConfig.buffer.size       = MQTT_BUFFER_SIZE;

    mqttConfig.credentials.username  = AWS_THINGNAME;
    mqttConfig.credentials.client_id = AWS_THINGNAME;

    mqttConfig.broker.verification.certificate     = AWS_ROOT_CERT;
    mqttConfig.broker.verification.certificate_len = sizeof(AWS_ROOT_CERT);

    mqttConfig.credentials.authentication.certificate     = AWS_CLIENT_CERTIFICATE;
    mqttConfig.credentials.authentication.certificate_len = sizeof(AWS_CLIENT_CERTIFICATE);

    mqttConfig.credentials.authentication.key     = AWS_CLIENT_PRIVATE_KEY;
    mqttConfig.credentials.authentication.key_len = sizeof(AWS_CLIENT_PRIVATE_KEY);

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

    LOG_INFO("m_semaphoreReadyToSubscribe address: %p", &m_semaphoreBrokerConnected);

    xSemaphoreTake(m_semaphoreBrokerConnected, portMAX_DELAY);

    if (esp_mqtt_client_subscribe(m_clientHandle, "helloWorld", 1) == MQTT_PUBLISH_ERROR)
    {
        LOG_ERROR("Error while attempting to subscribe topic");
    }

    if (esp_mqtt_client_publish(m_clientHandle, "helloWorld", "helloFromSIM7080G!", 0, 1, 0) == MQTT_PUBLISH_ERROR)
    {
        LOG_ERROR("Error while sending initial message");
    }

    while (true)
    {
        SLEEP_MS(30000);
        sendTelemetryMessage();
    }
}

void SimpleMqttClientController::sendTelemetryMessage()
{
    // in telemetry message we are sending randomized value in range -10°C to 40°C
    int temperature = (rand() % 50) - 10;

    std::string message = "{\"temperature\": " + std::to_string(temperature) + "}";

    if (esp_mqtt_client_publish(m_clientHandle, MQTT_TELEMETRY_TOPIC, message.c_str(), 0, 1, 0) == MQTT_PUBLISH_ERROR)
    {
        LOG_ERROR("Could not send telemetry message");
    }
}