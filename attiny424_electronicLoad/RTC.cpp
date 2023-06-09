#include "IncFile1.h"

void init_RTC(){
	
	RTC_CLKSEL = RTC_CLKSEL_INT32K_gc;
	
	RTC_PITCTRLA |= RTC_PITEN_bm | RTC_PERIOD_CYC32_gc; // pit flag with 1ms interval
	
	RTC_CMP = 100;
	
	while(RTC_STATUS & RTC_PERBUSY_bm){}; //wait for rtc to accept changes in register
		
	RTC_CTRLA |= RTC_RTCEN_bm | RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV32_gc;
		
	RTC_PER = 256;
}

void RTC_delay(uint16_t millis){
	
	uint16_t counter = 0;
	
	while(counter < millis) //idle loop
	{
		
		if(RTC_PITINTFLAGS)
		{
		
		counter +=1;
		
		RTC_PITINTFLAGS = RTC_PI_bm;
		
		}
		
	}
}