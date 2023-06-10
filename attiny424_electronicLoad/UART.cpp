#include "IncFile1.h"

void Init_USART1(){
	
	
	USART1_BAUD = (uint16_t)USART1_BAUD_RATE(115200);
	
	USART1_CTRLB |= USART_TXEN_bm; //Enable Tx
	
	
}

void USART1_sendChar(char c)
{
	while (!(USART1.STATUS & USART_DREIF_bm)){};

	USART1.TXDATAL = c;
}

void USART1_sendStr(const char *str){

	for(uint8_t i = 0; i<strlen(str); i++){

		USART1_sendChar(str[i]);
	}
	
	
}

char USART1_receive(){
	
	while (!(USART0_STATUS & 0x80)) {}
	
	return USART0.RXDATAL;
}

uint32_t USART1_toValue(const char *str) {
	uint32_t value = 0;
	uint8_t valueLength = strlen(str);
	uint32_t j = 1;
	
	if(valueLength>0){
		
		
		for (int8_t i = valueLength-1; i>=0; i--){
			
			if ((str[i] > 47 && str[i] < 58 )) { // is character digit?


				value += j * (uint8_t)(str[i] - '0'); //turn character to integer
				j *= 10;
			}
			else{
				
				value = 0; //non integer character found, return 0
				break;
			}
		}
	}
	return value;
}


void USART1_sendInt(int a, int8_t stuffing){
	const uint8_t bufSize = 12;
	char buffer[bufSize] = "";

	bool sign = 0;
	char *ptr = &buffer[bufSize-2];

	if(a == 0){
		USART1_sendChar('0');
		return;
	}
	if(a < 0){

		sign = 1;
		a = -a;
	}


	for(;a > 0;a/=10){

		*ptr = ('0'+ a % 10);
		ptr--;
		stuffing -=1;
	}
	for(;stuffing>0;stuffing--){

		*ptr = '0';
		ptr--;
	}

	if(sign)*ptr = '-'; else ptr++;

	

	USART1_sendStr(ptr);

}
