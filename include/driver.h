#ifndef FOGDEVICE_EMULATOR_DRIVER_H
#define FOGDEVICE_EMULATOR_DRIVER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#define MQTT_BROKER_URL_ENV_VAR     "MQTT_BROKER_URL"
#define MQTT_TOPIC_ENV_VAR          "MQTT_TOPIC"
#define MQTT_CLIENT_ID_ENV_VAR      "MQTT_CLIENT_ID"

#define SERIAL_FORMAT   "8N1"
#define BAUD_RATE       19200

#define USB0        0
#define USB0_NDX    16
#define USB1        1
#define USB1_NDX    17
#define USB2        2
#define USB2_NDX    18
#define USB3        3
#define USB3_NDX    19
#define USB4        4
#define USB4_NDX    20
#define USB5        5
#define USB5_NDX    21

#define CHANNEL_ANALOG_OUT_TEMPERATURE    20
#define CHANNEL_ANALOG_OUT_HUMIDITY       21
#define CHANNEL_ANALOG_OUT_PRESSURE       22

#define SPP_ID_WR_ANALOG  0x04

extern char * MQTT_BROKER_URL;
extern char * MQTT_TOPIC;
extern char * CLIENT_ID;

extern int port_id;
extern const uint8_t DUT_ADDR;

#define TEMPERATURE         'T'
#define HUMIDITY            'H'
#define PRESSURE            'P'
#define BMP280_DATA_LEN     6


void init(uint8_t com_port);
int loop();

#endif //FOGDEVICE_EMULATOR_DRIVER_H
