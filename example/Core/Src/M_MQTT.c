/*
 * M_MQTT.c
 *
 *  Created on: May 5, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 * 
 * for deep understanding from MQTT study https://www.hivemq.com/mqtt-essentials/
 */
#include "M_MQTT.h"


/**
 * @brief create MQTT Connect Packet and send to connected server
 * 
 * @param username 
 * @param password 
 * @param clientid 
 * @param keep_alive_interval 
 * @param clean_session 
 * 
 * @retval bool successful send connect packet to broker
 */
bool M_MQTT_connect(char *username, char *password, char *clientid,
		unsigned short keep_alive_interval, uint8_t clean_session) {
	unsigned char packet[64] = "";

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	data.username.cstring = username;
	data.password.cstring = password;
	data.clientID.cstring = clientid;
	data.keepAliveInterval = keep_alive_interval;
	data.cleansession = clean_session;

	int packet_len = MQTTSerialize_connect(packet, sizeof(packet), &data);

	return M_SIM800_sendData(packet, packet_len);
}


/**
 * @brief publish MQTT Message to a specific topic with packetid and QoS
 * 
 * @param topic 
 * @param payload 
 * @param packetid 
 * @param QoS 
 * 
 * @retval bool successful send publish packet to broker
 */
bool M_MQTT_publish(char *topic, char *payload, int packetid, int QoS) {
	unsigned char packet[256] = "";

	MQTTString topic_string = MQTTString_initializer;
	topic_string.cstring = topic;

	int packet_len = MQTTSerialize_publish(packet, sizeof(packet), 0, QoS, 0,
			packetid, topic_string, (unsigned char*) payload,
			(int) strlen(payload));

	return M_SIM800_sendData(packet, packet_len);
}


/**
 * @brief subscribe to topic with specific QoS
 * (Note: use QoS 0, because this library does not support PUBACK, SUBACK and others)
 * 
 * @param topic 
 * @param QoS 
 * 
 * @retval bool successful send subscribe packet to broker
 */
bool M_MQTT_subscribe(char *topic, int QoS) {
	static int packet_id = 100;
	unsigned char packet[256] = "";

	MQTTString topic_string[1] = { MQTTString_initializer };
	topic_string[0].cstring = topic;
	int qos[1] = { QoS };

	int packet_len = MQTTSerialize_subscribe(packet, sizeof(packet), 0, packet_id++, 1,
			topic_string, qos);

	return M_SIM800_sendData(packet, packet_len);
}


/**
 * @brief keeping alive connecting with broker with this 
 * 
 * @param None
 * 
 * @retval bool successful send ping req (keep alive packet) to broker
 */
bool M_MQTT_pingReq(void) {
	unsigned char packet[16] = "";

	int packet_len = MQTTSerialize_pingreq(packet, sizeof(packet));

	return M_SIM800_sendData(packet, packet_len);
}
