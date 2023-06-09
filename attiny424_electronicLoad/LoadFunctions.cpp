#include "IncFile1.h"


uint16_t setLoadmA(uint16_t mA){
	
	if(mA>constants::maxCurrentmA) mA = constants::maxCurrentmA;
	
	if ((mA > 200) && !Active0R1) //if load over 200 mA and 0R1 mosfet not active
	{ 

		setPwmDutycycle(0);
		RTC_delay(60); //60 ms delay to wait for load to turn off
		
		PORTA_OUTSET = pins::Mosfet0R1Pin_A;
		PORTA_OUTCLR = pins::Mosfet1R0Pin_A;
		Active0R1 = true;
		Active1R0 = false;
		RTC_delay(5);

	}
	else if((mA < 200) && !Active1R0) //if load less than 200mA and 0R1 is active,disable 0R1 and activate 1R0
	{

		setPwmDutycycle(0);
		RTC_delay(60);
		 
		 PORTA_OUTSET = pins::Mosfet1R0Pin_A;
		 PORTA_OUTCLR = pins::Mosfet0R1Pin_A;
		 Active1R0 = true;
		 Active0R1 = false;
		 RTC_delay(5);

	}
	float correction = 0;
	
	if(Active1R0) correction = constants::correction1R0; else correction = constants::correction0R1 * ( 0.0002 * (float)mA + 0.9668);
	
	float offsetTotal = constants::mAoffsetBase + (float)voltageReadDividerCurrentuA / 1000;
	
	correction = ((float)mA - offsetTotal ) * correction;
	
	uint16_t dutyCycle = (uint16_t)( ( (float)correction / (float)VDDmVoltage ) + 0.5 );
	
	setPwmDutycycle(dutyCycle);
	return dutyCycle;
}

