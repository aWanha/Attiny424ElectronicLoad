#include "IncFile1.h"


bool buttonAct(){
	
	uint16_t *variable_p;
	bool PressedFlag = false;

	
	switch(selectSetting){// selector for which variable is changed
				
		case 0:
		variable_p = &setmA;
		break;
				
		case 1:
		variable_p = &setResistance;
		break;
		
		case 2:
		variable_p = &mWatt;
		break;
					
		case 3:
		variable_p = &setMinVoltageTriggermV;
		break;
				
	}
			
	PORTB_PIN3CTRL = PORT_PULLUPEN_bm; //stop interrupts during button flag reading (only pullup resistor enabled)
	PORTB_PIN2CTRL = PORT_PULLUPEN_bm;
			
	if(!(PORTB_IN & pins::button1Pin_B) && !(PORTB_IN & pins::button2Pin_B)) buttonPressed = 3; //button state polling without interrupt, 3 = both pressed
			
	else if(!(PORTB_IN & pins::button1Pin_B))buttonPressed = 1;
			
	else if(!(PORTB_IN & pins::button2Pin_B))buttonPressed = 2;
	else {
				
		PORTB_PIN2CTRL |= PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc; // if buttons are not pressed during polling, activate button interrupts
		PORTB_PIN3CTRL |= PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc;
	}
			
	//Button functions
	if(buttonPressed == 3)//if both buttons
	{
		if(bothPressedTick<3) bothPressedTick += 1; //Count duration of both pressed, limit value to 4
		
		accelerationTick = 0; //reset accelerationCount to avoid high stepsize if quickly changing setting after holding both buttons.
	}
	else //check how long buttons were held upon release
	{
		if(0 < bothPressedTick)
		{
			if(bothPressedTick<3) // if less than 1 sec held, change setting page
			{
				if(selectSetting<3) selectSetting += 1; else selectSetting = 0; // 4 setting pages.
			}
			else //if pressed over second, reset settings
			{
				setResistance = 1000;
				setMinVoltageTriggermV = DUTmVoltage * 0.90;
				setmA = constants::minmASetting;
				mWatt = 0;
				energy = 0;
			}
			PressedFlag = true;	//flag to refresh print at higher rate when changing settings, given true status here to refresh page at next iteration when buttons relased
			
			USART1_sendStr("\r\n"); //newline to print selection section on clean line
		}	
		
		bothPressedTick = 0;
	}
		 
			
	if(buttonPressed == 1)//if 1st button
	{
		*variable_p += stepSize;
	}
	if(buttonPressed == 2)//if 2nd button
	{
		if(stepSize < *variable_p) *variable_p -= stepSize; else *variable_p = 0; //restrict 0 <= value
	}
			
	if(buttonPressed != 0) //if any button combination
	{
		PORTB_OUTTGL = pins::ledPin_B; //indication that button pressed
				
		buttonPressed = 0;
		
		PressedFlag = true;
		
		accelerationTick +=1;  //if button pressed over 4 iterations (1sec), start increasing step size
		if(accelerationTick > 3)
		{
			accelerationTick = 4;
					
			if(stepSize < constants::stepSizeMaxValue) stepSize +=5;
		}
				
	}else
	{
		accelerationTick = 0;
		stepSize = constants::stepSizeMinValue;
		PORTB_OUTCLR = pins::ledPin_B;
	}
	
	if(setmA <= constants::minmASetting) setmA = constants::minmASetting; //due to opamp offset voltage, min mA request is 5mA.
	return PressedFlag;
}