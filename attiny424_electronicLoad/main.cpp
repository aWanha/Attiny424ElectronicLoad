/*
 * ElectronicLoadAttiny424.cpp
 *
 * Created: 24.1.2023 8.23.25
 * Author : home
 */ 

#include "IncFile1.h"

uint16_t VDDmVoltage = 0;
uint16_t DUTmVoltage = 0;

uint8_t index;

uint8_t buttonPressed = 0;
uint8_t bothPressedTick = 0;
uint8_t selectSetting = 0;
uint8_t stepSize = constants::stepSizeMinValue;
uint8_t accelerationTick = 0;

bool Active1R0 = false;
bool Active0R1 = false;

uint16_t setmA = constants::minmASetting;
uint16_t setMinVoltageTriggermV = 0;
uint16_t setResistance = 1000;
uint16_t loadmA = 0;
uint16_t mWatt = 0;
uint32_t energy = 0;
uint16_t voltageReadDividerCurrentuA = 0;

int main(void)
{
	init_clock();
	init_ADC();
	Init_USART1();
	init_TimerA();
	init_pins();
	init_RTC();
	setPwmDutycycle(0);
	
	uint8_t reCalTimer = 81; //initial value to get instant calibration
	uint8_t Timer250ms = 0;

	bool minVoltageFlag = false;
	bool pressedFlag = false;
	
	while(!(RTC_INTFLAGS & RTC_OVF_bm)){};
		
	RTC_INTFLAGS = RTC_OVF_bm;
	
	PORTB_INTFLAGS = 0xFF;
	
	sei();
    while (1) 
    {
		
		if(RTC_INTFLAGS & RTC_OVF_bm) // overflow used for 250 ms interval
		{
			pressedFlag = buttonAct();
			
			voltageReadDividerCurrentuA = (float)DUTmVoltage * 1000 / ((float)constants::DUTVmeasureDivider * 1000); //uA
			
			Timer250ms +=1;
			reCalTimer +=1;
			RTC_INTFLAGS = RTC_OVF_bm;
		}
		
		if(reCalTimer > 80 && !minVoltageFlag) // recalibrate VDD every 80/4 = 20 sec
		{
			VDDmVoltage = (float)( ( ADCReadVDD() * constants::VDDmesErrortotal ) + 0.5);

			reCalTimer = 0;
		}
		
		if(Timer250ms > 3 || pressedFlag)
		{
			//1 second interval

			if(selectSetting == 3)//print only setMinVoltageTrigger page when changing this setting
			{

					USART1_sendStr("setMinVoltageTrigger: ");
					USART1_sendInt(setMinVoltageTriggermV,4);
			}
			
			else
			{
				if(selectSetting == 0)
				{
					USART1_sendStr("requestmA: ");
					USART1_sendInt(setmA,4);
				}else if (selectSetting == 1)
				{
					 USART1_sendStr("requestOhm: ");
					 USART1_sendInt(setResistance,4);
					 
				}else
				{
					 USART1_sendStr("requestmWatt, mA: ");
					 USART1_sendInt(loadmA,4);
				}
			
			
				USART1_sendStr(",  BatterymV: ");
				USART1_sendInt(VDDmVoltage,4);
			
				USART1_sendStr(",  DUTmV: ");
				USART1_sendInt(DUTmVoltage,4);
			
				if(minVoltageFlag)USART1_sendStr("***DUTMinVoltageTrigger***"); //error print replaces powermW
				
				else//if no MinvoltageError, print milliwatts normally
				{
					uint32_t powermW;
					if(selectSetting == 2)powermW = mWatt; //if milliwatt setting, print the requestedValue as mW
					else
					{
						powermW = (uint32_t)DUTmVoltage * (uint32_t)loadmA;
						powermW = (float)powermW / 1000;
					}			
					
					USART1_sendStr(",  PowermW: ");
					USART1_sendInt(powermW,4);
					energy += (powermW * Timer250ms) >> 2; // E = P * Timer250 (1/4 sec) / 4, calculation needed because button pressedFlag might run this part before 1 second passed
				}
					
				USART1_sendStr(",  EnergyJ: ");
				USART1_sendInt(energy / 1000 ,7);
				
				USART1_sendStr(",  minV: ");
				USART1_sendInt(setMinVoltageTriggermV,5);				
			}
			
			USART1_sendChar('\r');	

			if(!pressedFlag && !minVoltageFlag && (selectSetting != 3))USART1_sendChar('\n'); //dont print new line if min voltage reached or changing settings
			
			Timer250ms = 0;
			if(pressedFlag)minVoltageFlag = false;
				
			pressedFlag = false;
		}
		
		
		if(RTC_PITINTFLAGS && !minVoltageFlag) //use RTC PIT interrupt flag interval (1ms) for sampling rate. Stop actions if min voltage flag not cleared
		{
			DUTmVoltage = (float)( ( ADCRead() * constants::DUTmesErrorTotal ) + 0.5);// add half to round up for int value
			
			
			if( ( DUTmVoltage > setMinVoltageTriggermV ) || setMinVoltageTriggermV == 0 )
			{
				
				if(selectSetting == 1) //if resistance mode selected
				{
					if(setResistance > 0) loadmA = DUTmVoltage / setResistance;
					
				}else if(selectSetting == 2) //if watt mode selected
				{
					if(DUTmVoltage > 0) loadmA = 1000 * (uint32_t)mWatt / DUTmVoltage; else loadmA = constants::minmASetting;
					
				}else // if current mode selected
				{
					loadmA = setmA;
				}
				
				if(loadmA <= constants::minmASetting) loadmA = constants::minmASetting; //if mWatt mode or resistance mode request below min possible current, set mA to min value (this is only for sake of presentation in prints)
				
				if(loadmA > constants::maxCurrentmA) loadmA = constants::maxCurrentmA;
				
				if(DUTmVoltage > 100)
				{
					PORTA_DIRCLR = pins::disableLoadPin_A; //disable voltage feed into negative input of opamp by placing feeding pin into high impedance mode
					
					setLoadmA(loadmA);
					
				}else //safeguard to avoid requesting current without any supply leading to short circuit waiting when connecting supply
				{
					setPwmDutycycle(0);
					PORTA_DIRSET = pins::disableLoadPin_A; 
					PORTA_OUTSET = pins::disableLoadPin_A;//feed voltage into negative input of opamp to disable load driving
				}
			
			}
			else//if voltage drops  below min voltagevalue
			{
				setPwmDutycycle(0); 
				PORTA_DIRSET = pins::disableLoadPin_A;
				PORTA_OUTSET = pins::disableLoadPin_A;//feed voltage into negative input of opamp to disable load driving
				setmA = constants::minmASetting;
				setResistance = 1000;
				mWatt = 0;
				
				minVoltageFlag = true;
				
			}
			
			RTC_PITINTFLAGS = RTC_PI_bm;
		}
    }
}

ISR(PORTB_PORT_vect){//interrupt for button press
	
	if(PORTB_INTFLAGS & pins::button1Pin_B)
	{
		 buttonPressed += 1;
		 PORTB_PIN2CTRL = PORT_PULLUPEN_bm;
	}
	if(PORTB_INTFLAGS & pins::button2Pin_B)
	{
		 buttonPressed += 2;
		 PORTB_PIN3CTRL = PORT_PULLUPEN_bm;
	}

	PORTB_INTFLAGS = 0xFF;
}
