#include "mqtt_client.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

class SimpleMqttClientController
{
    constexpr static char MQTT_BROKER_ADDRESS[] = "mqtt://0.tcp.eu.ngrok.io";
    constexpr static char MQTT_CLIENT_ID[] = "";
    constexpr static char MQTT_USERNAME[] = "someReallyRandomUser1";
    constexpr static char MQTT_PASSWORD[] = "someReallyRandomPassword";

    constexpr static int SIMPLE_MQTT_CONTROLLER_STACK_SIZE = 6144;

    constexpr static int MQTT_BUFFER_SIZE = 4096;
    constexpr static int MQTT_OUTBOX_LIMIT_BYTES = 4096;
    constexpr static int MQTT_PORT = 18213;
    constexpr static int MQTT_KEEPALIVE_VALUE = 60;

    constexpr static int MQTT_PUBLISH_ERROR = -1;
    constexpr static int MQTT_SUBSCRIPTION_ERROR = -1;

public:
    SimpleMqttClientController();
    void runTask();
    static void run(void *pObject);
    void _run();
    void mqttEventHandler(void *handlerArgs, esp_event_base_t base, int32_t eventId, void *eventData);

private:
    void init();
    void perform();

    esp_mqtt_client_handle_t m_clientHandle;
    TaskHandle_t m_taskHandle;
    SemaphoreHandle_t m_semaphoreBrokerConnected;
};