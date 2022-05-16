/*
 * M_MQTT.h
 *
 *  Created on: May 5, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 * 
 *  for deep understanding from MQTT study https://www.hivemq.com/mqtt-essentials/
 */

#ifndef INC_M_MQTT_H_
#define INC_M_MQTT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "M_SIM800.h"
#include "MQTTPacket.h"

/* Functions -----------------------------------------------------------------*/
bool M_MQTT_connect(char *username, char *password, char *clientid, unsigned short keep_alive_interval, uint8_t clean_session);
bool M_MQTT_publish(char *topic, char *payload, int packetid, int QoS);
bool M_MQTT_subscribe(char *topic, int QoS);
bool M_MQTT_pingReq(void);

/* ---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern }
#endif

#endif /* INC_M_MQTT_H_ */
