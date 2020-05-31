#include "rs232.h"
#include "MQTTAsync.h"
#include "driver.h"
#include "mqtt.h"
#include "spp/spp.h"

char * MQTT_BROKER_URL = "tcp://127.0.0.1:1883";
char * MQTT_TOPIC = "fogdevices/x/slave/#";
char * CLIENT_ID = "x";

int port_id;
const uint8_t DUT_ADDR = 0x30;

void init(const uint8_t com_port)
{
    MQTT_BROKER_URL = getenv(MQTT_BROKER_URL_ENV_VAR);
    MQTT_TOPIC = getenv(MQTT_TOPIC_ENV_VAR);
    CLIENT_ID = getenv(MQTT_CLIENT_ID_ENV_VAR);

    switch (com_port) {
        case USB0:
            port_id = USB0_NDX;
            break;
        case USB1:
            port_id = USB1_NDX;
            break;
        case USB2:
            port_id = USB2_NDX;
            break;
        case USB3:
            port_id = USB3_NDX;
            break;
        case USB4:
            port_id = USB4_NDX;
            break;
        case USB5:
            port_id = USB5_NDX;
            break;
        default:
            printf("Unsupported port. Supported ports are from range 0-5\n");
            exit(1);
    }
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    T_sppPacket packet;
    uint8_t active_channel = 0;
    float value = 0;
    uint16_t analog_value;
    int16_t signed_analog_value;

    if (message->payloadlen < 8) {
        return 1;
    }

    value = strtof(&((char *) message->payload)[8], NULL);
    signed_analog_value = value * 10;
    analog_value = signed_analog_value;

    switch (((char*) message->payload)[7]) {
        case HUMIDITY:
            active_channel = CHANNEL_ANALOG_OUT_HUMIDITY;
            break;
        case TEMPERATURE:
            active_channel = CHANNEL_ANALOG_OUT_TEMPERATURE;
            break;
        case PRESSURE:
            active_channel = CHANNEL_ANALOG_OUT_PRESSURE;
            break;
    }

    if (active_channel > 0) {
        printf("Send value %u for channel %u\n", analog_value, active_channel);
        packet.dstAddr = DUT_ADDR;
        packet.len = 3;
        packet.cmdid = SPP_ID_WR_ANALOG;
        packet.payload[0] = active_channel;
        packet.payload[1] = analog_value>>8;
        packet.payload[2] = analog_value;
        sppTx(&packet);
    }

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

void onConnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsync client = (MQTTAsync)context;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    printf("Successful connection\n");

    printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
           "Press Q<Enter> to quit\n\n", MQTT_TOPIC, CLIENT_ID, QOS);
    opts.onSuccess = onSubscribe;
    opts.onFailure = onSubscribeFailure;
    opts.context = client;
    if ((rc = MQTTAsync_subscribe(client, MQTT_TOPIC, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start subscribe, return code %d\n", rc);
        finished = 1;
    }
}

int loop()
{
    int ch, rc;
    MQTTAsync client;

    if (RS232_OpenComport(port_id,BAUD_RATE, SERIAL_FORMAT) != 0)
    {
        printf("Cannot open com port\n");
        return 1;
    };

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;

    if ((rc = MQTTAsync_create(&client, MQTT_BROKER_URL, CLIENT_ID, MQTTCLIENT_PERSISTENCE_NONE, NULL))
        != MQTTASYNC_SUCCESS)
    {
        printf("Failed to create MQTT client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto exit;
    }

    if ((rc = MQTTAsync_setCallbacks(client, client, connlost, msgarrvd, NULL)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = client;
    if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    while (!subscribed && !finished)
    {
        usleep(10000L);
    }

    if (finished)
        goto exit;

    do
    {
        ch = getchar();
        sppProcess();
    } while (ch!='Q' && ch != 'q');

    disc_opts.onSuccess = onDisconnect;
    disc_opts.onFailure = onDisconnectFailure;
    if ((rc = MQTTAsync_disconnect(client, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }
    while (!disc_finished)
    {
        usleep(10000L);
    }

    destroy_exit:
    MQTTAsync_destroy(&client);
    exit:
    RS232_CloseComport(port_id);
    return rc;
}
