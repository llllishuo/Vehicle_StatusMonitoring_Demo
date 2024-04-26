#include "./bsp_dht11.h"


void DHT11_Init(){
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	DHT11_GPIO_APBxClkCmd(DHT11_GPIO_CLK, ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Pin = DHT11_GPIO_PIN;
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitTypeStruct);
	DHT11_GPIO_WriteBit(Bit_High);
}
void DHT11_Mode_Out_PP(){
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	GPIO_InitTypeStruct.GPIO_Pin = DHT11_GPIO_PIN;
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitTypeStruct);
}
void DHT11_Mode_IPU(){
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	GPIO_InitTypeStruct.GPIO_Pin = DHT11_GPIO_PIN;
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(DHT11_GPIO_PORT, &GPIO_InitTypeStruct);
}
static uint8_t DHT11_Read8bit(){
	uint8_t i = 0;
	uint8_t temp = 0;
	//printf("[Debug] dyte: ");
	for(i = 0; i < 8; i++){
		//每位以50us低电平开始，轮询直到传感器发送50us的低电平
		while(DHT11_GPIO_ReadBit() == Bit_Low);
		// 26-28us高电平为0 70us高电平为1
		// 根据 x us后电平位置判断，如果为高电平则1，低电平则0
		udelay(4);
		if( DHT11_GPIO_ReadBit() == Bit_High){
			//等待高电平结束
			while(DHT11_GPIO_ReadBit() == Bit_High);
			temp |= (uint8_t)(0x01 << (7-i));
			//printf("1");
		}else{//如果低电平则表示为0  
			temp &= (uint8_t)~(0x01 << (7-i));
			//printf("0");
		}
		
			
	}
	//printf("\n");
	return temp;
}

uint8_t DHT11_Read_TempAndHumi_ToStruct(DHT11_TypeDef *DHT11_TypedefStruct){
	
	DHT11_Mode_Out_PP(); // 推挽输出
	// 拉低
	DHT11_GPIO_WriteBit(Bit_Low);
	// 延时18ms
	mdelay(18);
	// 拉高
	DHT11_GPIO_WriteBit(Bit_High);
	// 延时需要20-40us
	udelay(2);
	
	DHT11_Mode_IPU(); // 上拉电阻输入
	
	// 判断传感器是否有低电平的响应信号，如果不响应则跳出，响应则先下运行
	if(DHT11_GPIO_ReadBit() == Bit_Low){
		//printf("[Debug]0.响应信号: %d\n", DHT11_GPIO_ReadBit());
		// 等待80us低电平的响应信号
		while(DHT11_GPIO_ReadBit() == Bit_Low);
		//Delay_us(80);
		//printf("[Debug]1.响应信号: %d\n", DHT11_GPIO_ReadBit());
		// 等待80us高电平的响应信号
		while(DHT11_GPIO_ReadBit() == Bit_High);
		//Delay_us(80);
		//printf("[Debug]2.响应信号: %d\n", DHT11_GPIO_ReadBit());
		
		
		// 接收数据
		DHT11_TypedefStruct->humi_int = DHT11_Read8bit();
		DHT11_TypedefStruct->humi_dect = DHT11_Read8bit();	
		DHT11_TypedefStruct->temp_int = DHT11_Read8bit();
		DHT11_TypedefStruct->temp_dect = DHT11_Read8bit();
		// 校验位
  	DHT11_TypedefStruct->check_sum = DHT11_Read8bit();
		// 恢复初始状态
		// 输出模式
		DHT11_Mode_Out_PP();
		// 拉高
		DHT11_GPIO_WriteBit(Bit_High);
		/*
		uint8_t temp = DHT11_TypedefStruct->humi_int  
									+ DHT11_TypedefStruct->humi_dect
									+ DHT11_TypedefStruct->temp_int 
									+ DHT11_TypedefStruct->temp_dect;

		printf("temp(%d) -- check_sum(%d)\n", temp, DHT11_TypedefStruct->check_sum);
		if(temp == DHT11_TypedefStruct->check_sum){
			return Ok;
		}
		*/
		return Ok;
	}
	return Err;

	
	
}
