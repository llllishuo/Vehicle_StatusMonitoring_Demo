#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__

#include "stm32f10x.h"                  // Device header
#include "./SysTick/bsp_SysTick.h"
#include "stdio.h"

#define DHT11_GPIO_CLK RCC_APB2Periph_GPIOD
#define DHT11_GPIO_APBxClkCmd RCC_APB2PeriphClockCmd

#define DHT11_GPIO_PORT GPIOD
#define DHT11_GPIO_PIN GPIO_Pin_6

#define Bit_High Bit_SET
#define Bit_Low Bit_RESET

#define Ok 1;
#define Err 0;

#define DHT11_GPIO_ReadBit() GPIO_ReadInputDataBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN)

#define DHT11_GPIO_WriteBit(KEY) GPIO_WriteBit(DHT11_GPIO_PORT, DHT11_GPIO_PIN, KEY)

typedef struct{

	// 湿度
	uint8_t humi_int;
	uint8_t humi_dect;
	// 温度
	uint8_t temp_int;
	uint8_t temp_dect;
	
	// 检验和
	uint8_t check_sum;
	
}DHT11_TypeDef;

void DHT11_Init();
uint8_t DHT11_Read_TempAndHumi_ToStruct(DHT11_TypeDef *DHT11_TypedefStruct);



#endif