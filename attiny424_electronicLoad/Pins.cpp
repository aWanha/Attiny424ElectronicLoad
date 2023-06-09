#include "IncFile1.h"

void init_pins(){
	
	//By default all digitalBuffers disabled
	
	PORTA_PIN1CTRL; //uart pin
	PORTA_PIN2CTRL; //uart pin
	PORTA_PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTA_PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc; // voltage read of dut
	PORTA_PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc; //disable load voltage, quick fix to feed voltage into opamp negative input to prevent short when attaching load
	PORTA_PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc; //mosfet control
	PORTA_PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc; //mosfet control
	
	PORTB_PIN0CTRL = PORT_ISC_INPUT_DISABLE_gc; //PWM output
	PORTB_PIN1CTRL = PORT_ISC_INPUT_DISABLE_gc; //Led
	PORTB_PIN2CTRL |= PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc; //button1
	PORTB_PIN3CTRL |= PORT_PULLUPEN_bm | PORT_ISC_LEVEL_gc; //button 2


	
	
	PORTA_DIRSET |= PIN1_bm | pins::Mosfet1R0Pin_A | pins::Mosfet0R1Pin_A; // 1 as uart TX
	
	PORTB_DIRSET |= PIN0_bm | pins::ledPin_B;
}