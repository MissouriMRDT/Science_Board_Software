/*
 * DS18B20.c
 *
 *  Created on: Oct 25, 2026
 *      Author: gavin
 */


#include "DS18B20.h"

float interpretTemp(int16_t input)
{
	for (int i = 1; i < INTERP_NUM; i++)
	{
		if (input > interpCheckpoints[i])
			return interpResults[i] + ((interpResults[i-1] - interpResults[i]) * (((float)input - (float)interpCheckpoints[i])/((float)interpCheckpoints[i-1] - (float)interpCheckpoints[i])));
	}
	return -55;
}

uint8_t busResetDS18B20(UART_HandleTypeDef* uart)
{
	HAL_UART_DeInit(uart);
	uart->Init.BaudRate = 6900;
	if (HAL_UART_Init(uart) != HAL_OK) return 1; //set baude failed

	uint8_t tempData = 0xF0;

	HAL_UART_Transmit(uart, &tempData, 1, 100);
	if (HAL_UART_Receive(uart, &tempData, 1, 1000) != HAL_OK) return 2; //recieve failed.
	if (tempData == 0xF0) return 3; //no response

	HAL_UART_DeInit(uart);
	uart->Init.BaudRate = 115200;
	if (HAL_UART_Init(uart) != HAL_OK) return 4; //set baude failed 2

	return 0; //pinged succesfully.
}

uint8_t writeDS18B20(UART_HandleTypeDef* uart, uint8_t data)
{
	uint8_t buffer[8];
	for (int i=0; i < 8; i++)
	{
		if ((data & (1 << i)) != 0)
			buffer[i] = 0xFF;
		else
			buffer[i] = 0x00;
	}
	if (HAL_UART_Transmit(uart, buffer, 8, 100) != HAL_OK) return 1; //failed to send

	return 0; //sent successfully.
}

uint8_t readDS18B20(UART_HandleTypeDef* uart, uint8_t* data)
{
	uint8_t out = 0b11111111;
	uint8_t in = 0;
	uint8_t value = 0;

	for (int i = 0; i < 8; i++)
	{
		if (HAL_UART_Transmit_DMA(uart, &out, 1) != HAL_OK) return i + 1; //failed i send
		for (int a = 0; a < 500; a++)
		{
			__NOP();
		}
		GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);

		in = (pinState == GPIO_PIN_SET ? 1 : 0);
		value |= (in << i);
		HAL_Delay(5);
	}

	*data = value;

	return 0; //received successfully.
}

uint8_t takeTempDS18B20(UART_HandleTypeDef* uart, float* temp)
{
	uint8_t tempL, tempM;

	if (busResetDS18B20(uart) != 0) return 1; //failed to reset bus 1
	if (writeDS18B20(uart, 0xCC) != 0) return 2; //failed to send skip ROM command
	if (writeDS18B20(uart, 0x44) != 0) return 3; //failed to convert temperature.
	if (busResetDS18B20(uart) != 0) return 4;
	if (writeDS18B20(uart, 0xCC) != 0) return 5; //failed to send skip ROM
	if (writeDS18B20(uart, 0xBE) != 0) return 6; //failed to send read command
	if (readDS18B20(uart, &tempL) != 0) return 7; //failed to read LSB
	if (readDS18B20(uart, &tempM) != 0) return 8; //failed to read MSB

	*temp = interpretTemp((uint16_t)((tempM << 8) | tempL)); // convert temperature output to C.
	return 0;
}
