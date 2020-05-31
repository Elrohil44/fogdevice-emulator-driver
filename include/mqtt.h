#ifndef FOGDEVICE_EMULATOR_DRIVER_MQTT_H
#define FOGDEVICE_EMULATOR_DRIVER_MQTT_H

#include "MQTTAsync.h"

extern int disc_finished;
extern int subscribed;
extern int finished;

#define QOS 1

void connlost(void *context, char *cause);
void onDisconnectFailure(void* context, MQTTAsync_failureData* response);
void onDisconnect(void* context, MQTTAsync_successData* response);
void onSubscribe(void* context, MQTTAsync_successData* response);
void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
void onConnectFailure(void* context, MQTTAsync_failureData* response);
void onConnect(void* context, MQTTAsync_successData* response);

#endif //FOGDEVICE_EMULATOR_DRIVER_MQTT_H
