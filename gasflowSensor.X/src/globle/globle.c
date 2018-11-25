#include "../../mcc_generated_files/mcc.h"
#include "globle.h"
volatile unsigned char	isRunInBoot	@ 0xfff;
volatile uint16_t globalTicker=0x00ul;
volatile uint8_t Event=0;  
/*
void delay(uint16_t dly)
{
    while(dly--){
		__nop();
		__nop();
		__nop();
		__nop();
		
    }
}
void delay_ms(uint16_t dly)
{
    uint16_t t16;
    t16=dly/20;
    if(t16==0)t16=1;
    do{
        delay(__20ms);
    }while(t16--);
}

*/