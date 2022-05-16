/*
 * M_SIM800.h
 *
 *  Created on: May 5, 2022
 *      Author: Masoud Heidary
 *      gitHub: https://github.com/MasoudHeidary/
 *     License: MIT
 */

#ifndef INC_M_SIM800_H_
#define INC_M_SIM800_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define __M_SIM800_UART &huart1
#define __M_SIM800_OS false
#define __M_SIM800_Delay 2000

#define __M_SIM800_BufLen 100

/* Variables -----------------------------------------------------------------*/
// use in interrupt to get income RX char
uint8_t __M_SIM800_RXData;

// save income save ("__M_SIM800_RXData") in this array
char __M_SIM800_RXBuf[__M_SIM800_BufLen];
int __M_SIM800_RXBufPointer;

char __M_SIM800_RepliedAnswer[__M_SIM800_BufLen];

/* Functions -----------------------------------------------------------------*/
void M_SIM800_RXCallBack(void);
void M_SIM800_incomePacket(void);

bool M_SIM800_sendCommand(char *command, char *answer, uint16_t delay);
bool M_SIM800_sendData(uint8_t *data, int data_len);

bool M_SIM800_init(void);
bool M_SIM800_connect(char *APN, char *APN_user, char *APN_pass, char *host, uint16_t port);

/* ---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern }
#endif

#endif /* INC_M_SIM800_H_ */
