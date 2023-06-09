#include "IncFile1.h"

void init_ADC(){
	
	ADC0_CTRLA |= ADC_ENABLE_bm;
	
	ADC0_CTRLB = ADC_PRESC_DIV6_gc;
	
	ADC0_CTRLC |= ADC_REFSEL_2048MV_gc | 31 << 3; 
	
	ADC0_CTRLF = ADC_SAMPNUM_ACC16_gc;
	
	ADC0_CTRLE |= 20;
	
	ADC0_MUXNEG = 0;
	
	ADC0_COMMAND |= ADC_MODE_BURST_gc;
	
}


uint16_t ADCReadVDD(){
	
	ADC0_MUXPOS = ADC_MUXPOS_VDDDIV10_gc;
	
	RTC_delay(5); //settling time
	ADC0_CTRLF = ADC_SAMPNUM_ACC256_gc; // 256 samples
	
	uint16_t result11Bit = ADCRead() >> 4; // bitshift to 2048 resolution
	
	ADC0_MUXPOS = pins::DUTanalogPin; //change settings back
	ADC0_CTRLF = ADC_SAMPNUM_ACC16_gc;
	
	RTC_delay(5); //settling time
	
	return result11Bit;
}


void ADCSetWindowGuard(uint16_t min){
	
	ADC0_WINLT = min;
	
	ADC0_INTCTRL |= ADC_WCMP_bm;
}


uint16_t ADCRead(){
	
		ADC0_COMMAND |= ADC_START_IMMEDIATE_gc;
		
		while( !( ADC0_INTFLAGS & ADC_RESRDY_bm ) ){};
		
		uint16_t result = (ADC0_RESULT >> 5 );
		
		ADC0_INTFLAGS |= ADC_RESRDY_bm;
		
		return result - constants::ADCoffsetError;
}