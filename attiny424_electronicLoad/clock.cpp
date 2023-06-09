#include "IncFile1.h"

void init_clock(){


	CCP = 0xD8; //unlock write protection
	
	CLKCTRL_MCLKCTRLB = CLKCTRL_PEN_bm; //clock 10 MHz
	
	
}