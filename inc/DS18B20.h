/*
 * DS18B20.h
 *
 *  Created on: Oct 25, 2026
 *      Author: gavin
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "main.h"

#define INTERP_NUM 10
static const int16_t interpCheckpoints[INTERP_NUM] = {0x07D0, 0x0550, 0x0191, 0x00A2, 0x0008, 0x0000, 0xFF8, 0xFF5E, 0xFE6F, 0xFC90};
static const float interpResults[INTERP_NUM] = {125, 85, 26.0625, 10.125, .5, 0, -.5, -10.125, -25.0625, -55};

float interpretTemp(int16_t input);
uint8_t busResetDS18B20(UART_HandleTypeDef* uart);
uint8_t writeDS18B20(UART_HandleTypeDef* uart, uint8_t data);
uint8_t readDS18B20(UART_HandleTypeDef* uart, uint8_t* data);
uint8_t takeTempDS18B20(UART_HandleTypeDef* uart, float* temp);

#endif /* INC_DS18B20_H_ */
