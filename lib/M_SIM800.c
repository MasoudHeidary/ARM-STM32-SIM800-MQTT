/*
 * M_SIM800.c
 *
 *  Created on: May 5, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */


#include "M_SIM800.h"

/**
 * @brief Manage RX Buffer - use this function in RX Interrupt CallBack Function("HAL_UART_RxCpltCallback")
 * 
 * @param None
 * 
 * @retval None
 */
void M_SIM800_RXCallBack(void) {
	__M_SIM800_RXBuf[__M_SIM800_RXBufPointer++] = __M_SIM800_RXData;
	__M_SIM800_RXBufPointer %= __M_SIM800_BufLen;
	for (int i = 0; i < __M_SIM800_RXBufPointer; i++)
		if (__M_SIM800_RXBuf[i] == 0)
			__M_SIM800_RXBuf[i] = 1;

	if (strstr((char *) __M_SIM800_RXBuf, "\r\n") && __M_SIM800_RXBufPointer == 2)
		__M_SIM800_RXBufPointer = 0;

	else if(strstr((char *) __M_SIM800_RXBuf, "\r\n") != NULL) {
		if(strstr((char *) __M_SIM800_RXBuf, "$@") != NULL) {
			M_SIM800_incomePacket();
		}
		else
			memcpy(__M_SIM800_RepliedAnswer, __M_SIM800_RXBuf, __M_SIM800_BufLen);
		__M_SIM800_RXBufPointer = 0;
		memset(__M_SIM800_RXBuf, 0, __M_SIM800_BufLen);
	}

	__HAL_UART_CLEAR_OREFLAG(__M_SIM800_UART);
	HAL_UART_Receive_IT(__M_SIM800_UART, &__M_SIM800_RXData, 1);
}


/**
 * @brief process incoming message (could be MQTT) and update state
 * 
 * @param None
 * 
 * @retval None
 */
void M_SIM800_incomePacket(void) {
	// add your code 

	// -----EXAMPLE
	// char income_command = 0;
	// sscanf(strstr((char *) __M_SIM800_RXBuf, "$@"), "$@%c", &income_command);

	// //validate income command
	// if ((income_command & 0b00111000) != 0b00111000)
	// 	return;
	// if ((income_command & 0b11000000) != 0b00000000)
	// 	return;

	// //execute income command
	// char lock = income_command & 0b00000011;
	// char front_led = (income_command & 0b00000100) >> 2;

	// HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, lock);
	// -----EXAMPLE END

	return;
}


/**
 * @brief send command to Sim800 and validate replied answer
 * 
 * @param command send this string to Sim800
 * @param answer if replied answer contain this return true otherwise return false
 * @param delay wait for Sim800 for answer to command
 * 
 * @retval bool Sim800 replied as expect or not
 */
bool M_SIM800_sendCommand(char *command, char *answer, uint16_t delay) {
	HAL_UART_Transmit_IT(__M_SIM800_UART, (uint8_t *)command, (uint16_t) strlen(command));

#if __M_SIM800_OS == 1
	osDelay(delay)
#else
	HAL_Delay(delay);
#endif

	if(strstr(__M_SIM800_RepliedAnswer, answer) != NULL) {
		memset(__M_SIM800_RepliedAnswer, 0 , __M_SIM800_BufLen);
		HAL_Delay(1000);
		return true;
	}
	return false;
}


/**
 * @brief send data to connected server (before this function use M_SIM800_connect to connect to a server)
 * 
 * @param data 
 * @param data_len 
 * 
 * @retval bool data send to server successfully (failed? Internet Connection - Server Connection - SimCard Charge)
 */
bool M_SIM800_sendData(uint8_t *data, int data_len) {
	char str[32] = "";
	sprintf(str, "AT+CIPSEND=%d\r\n", data_len);
	M_SIM800_sendCommand(str, ">", 200);

	HAL_UART_Transmit_IT(__M_SIM800_UART, data, data_len);

	M_SIM800_sendCommand(" ", "SEND OK", __M_SIM800_Delay);
	return true;
}


/**
 * @brief initialize Sim800 and RX Interrupt
 * 
 * @retval bool Sim800 initialized successfully (failed? Sim800 Connected - Power)
 */
bool M_SIM800_init(void) {
	HAL_UART_Receive_IT(__M_SIM800_UART, &__M_SIM800_RXData, 1);

	if(!M_SIM800_sendCommand("AT\r\n", "OK", __M_SIM800_Delay))
		return false;

	if(!M_SIM800_sendCommand("ATE0\r\n", "OK", __M_SIM800_Delay))
		return false;

	return true;
}


/**
 * @brief connect to a specific server
 * 
 * @param APN APN of your SimCard
 * @param APN_user usually empty ""
 * @param APN_pass usually empty ""
 * @param host domain address or IP to connect to it
 * @param port port of connection
 * 
 * @retval bool successful connection to server
 */
bool M_SIM800_connect(char *APN, char *APN_user, char *APN_pass, char *host, uint16_t port) {
	if (! M_SIM800_sendCommand("AT+CIPSHUT\r\n", "SHUT OK", __M_SIM800_Delay))
			return false;

	if(! M_SIM800_sendCommand("AT+CGATT=1\r\n", "OK", __M_SIM800_Delay))
		return false;

	char str[64] = "";
	snprintf(str, sizeof(str), "AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n", APN, APN_user, APN_pass);
	if(! M_SIM800_sendCommand(str, "OK", __M_SIM800_Delay))
		return false;

	if(! M_SIM800_sendCommand("AT+CIICR\r\n", "OK", __M_SIM800_Delay))
		return false;

	M_SIM800_sendCommand("AT+CIFSR\r\n", ".", __M_SIM800_Delay);


	memset(str, 0, sizeof(str));
	sprintf(str, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", host, port);
	if(! M_SIM800_sendCommand(str, "CONNECT OK", __M_SIM800_Delay))
		return false;

	return true;
}
