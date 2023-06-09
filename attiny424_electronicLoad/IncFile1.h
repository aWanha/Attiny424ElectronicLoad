/*
 * IncFile1.h
 *
 * Created: 24.1.2023 8.24.30
 *  Author: home
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>

#define F_CPU 10000000
#define USART1_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 *(float)BAUD_RATE)) + 0.5)

namespace pins{
	
constexpr uint8_t Mosfet1R0Pin_A = PIN7_bm;
constexpr uint8_t ledPin_B = PIN1_bm;
constexpr uint8_t Mosfet0R1Pin_A = PIN6_bm;
constexpr uint8_t DUTanalogPin = ADC_MUXPOS_AIN4_gc;
constexpr uint8_t button1Pin_B = PIN2_bm;
constexpr uint8_t button2Pin_B = PIN3_bm;
constexpr uint8_t disableLoadPin_A = PIN5_bm;
}

namespace constants{
	
constexpr uint8_t minmASetting = 5;
constexpr uint8_t stepSizeMinValue = 1;
constexpr uint8_t stepSizeMaxValue = 200;	

constexpr uint16_t pwmResolution = 4527; // resolution value to have best match in rounding
constexpr uint16_t maxCurrentmA = 2000;

constexpr float Resistor1R0 = 1.16071f;
constexpr float Resistor0R1 = 0.14409f;
constexpr float mAoffsetBase = 1.32f;
constexpr float opAmpVdivider = 5.22377f;
constexpr float correction1R0 = (pwmResolution*opAmpVdivider*Resistor1R0);
constexpr float correction0R1 = (pwmResolution*opAmpVdivider*Resistor0R1);
constexpr float ADCoffsetError = 2.0;
constexpr float ref2048Error = 0.9927f;
constexpr float DUTVmeasureDivider = 5.27717f;
constexpr float VDDmesErrortotal = (9.83928f * ref2048Error);
constexpr float DUTmesErrorTotal = (ref2048Error * DUTVmeasureDivider);
}

extern bool Active1R0;
extern bool Active0R1;

extern uint8_t buttonPressed;
extern uint8_t bothPressedTick;
extern uint8_t selectSetting;
extern uint8_t stepSize;
extern uint8_t accelerationTick;
extern uint8_t index;

extern uint16_t voltageReadDividerCurrentuA;
extern uint16_t VDDmVoltage;
extern uint16_t setMinVoltageTriggermV;
extern uint16_t setResistance;
extern uint16_t DUTmVoltage;
extern uint16_t setmA;
extern uint16_t mWatt;

extern uint32_t energy;


void init_clock();

void init_pins();

void init_ADC();

void init_RTC();

void init_TimerA();

void Init_USART1();

void ADCSetWindowGuard(uint16_t min);

uint16_t ADCRead();

uint16_t ADCReadVDD();

void USART1_sendChar(char c);

void USART1_sendStr(const char *str);

char USART1_receive();

void USART1_sendInt(int a, int8_t stuffing);

uint32_t USART1_toValue(const char *str);

void setPwmDutycycle(uint16_t dutyCycle);

uint16_t setLoadmA(uint16_t mA);

void RTC_delay(uint16_t millis);

bool buttonAct();


#endif /* INCFILE1_H_ */