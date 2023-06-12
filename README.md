# Attiny424ElectronicLoad
Attiny424 controlled dc load. Basic features such as constant current, resistance and power. Can measure battery energy and remove load when dut voltage drops below defined threshold level. Load can be set in 1mA increments within 2%, main cause of bad accuracy is the cheap op amp and using it near 0v rail. Inconsistent linearity is corrected in software with matching slope. Interface through buttons and serial communication


![schematic](https://github.com/aWanha/Attiny424ElectronicLoad/blob/main/schematic.jpg)
