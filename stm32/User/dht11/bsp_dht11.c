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
		//ÿλ��50us�͵�ƽ��ʼ����ѯֱ������������50us�ĵ͵�ƽ
		while(DHT11_GPIO_ReadBit() == Bit_Low);
		// 26-28us�ߵ�ƽΪ0 70us�ߵ�ƽΪ1
		// ���� x us���ƽλ���жϣ����Ϊ�ߵ�ƽ��1���͵�ƽ��0
		udelay(4);
		if( DHT11_GPIO_ReadBit() == Bit_High){
			//�ȴ��ߵ�ƽ����
			while(DHT11_GPIO_ReadBit() == Bit_High);
			temp |= (uint8_t)(0x01 << (7-i));
			//printf("1");
		}else{//����͵�ƽ���ʾΪ0  
			temp &= (uint8_t)~(0x01 << (7-i));
			//printf("0");
		}
		
			
	}
	//printf("\n");
	return temp;
}

uint8_t DHT11_Read_TempAndHumi_ToStruct(DHT11_TypeDef *DHT11_TypedefStruct){
	
	DHT11_Mode_Out_PP(); // �������
	// ����
	DHT11_GPIO_WriteBit(Bit_Low);
	// ��ʱ18ms
	mdelay(18);
	// ����
	DHT11_GPIO_WriteBit(Bit_High);
	// ��ʱ��Ҫ20-40us
	udelay(2);
	
	DHT11_Mode_IPU(); // ������������
	
	// �жϴ������Ƿ��е͵�ƽ����Ӧ�źţ��������Ӧ����������Ӧ����������
	if(DHT11_GPIO_ReadBit() == Bit_Low){
		//printf("[Debug]0.��Ӧ�ź�: %d\n", DHT11_GPIO_ReadBit());
		// �ȴ�80us�͵�ƽ����Ӧ�ź�
		while(DHT11_GPIO_ReadBit() == Bit_Low);
		//Delay_us(80);
		//printf("[Debug]1.��Ӧ�ź�: %d\n", DHT11_GPIO_ReadBit());
		// �ȴ�80us�ߵ�ƽ����Ӧ�ź�
		while(DHT11_GPIO_ReadBit() == Bit_High);
		//Delay_us(80);
		//printf("[Debug]2.��Ӧ�ź�: %d\n", DHT11_GPIO_ReadBit());
		
		
		// ��������
		DHT11_TypedefStruct->humi_int = DHT11_Read8bit();
		DHT11_TypedefStruct->humi_dect = DHT11_Read8bit();	
		DHT11_TypedefStruct->temp_int = DHT11_Read8bit();
		DHT11_TypedefStruct->temp_dect = DHT11_Read8bit();
		// У��λ
  	DHT11_TypedefStruct->check_sum = DHT11_Read8bit();
		// �ָ���ʼ״̬
		// ���ģʽ
		DHT11_Mode_Out_PP();
		// ����
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
