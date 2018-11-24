#include "../../mcc_generated_files/mcc.h"
#include "globle.h"
volatile uint32_t globalTicker=0x00ul;
volatile uint8_t Event=0;  
void delay_us(uint16_t dly)
{
    while(dly){
        __nop();__nop();__nop();
        dly--;
    }
    __nop();
}
void delay_ms(uint16_t dly)
{
    while(dly){
    delay_us(1000);
    dly--;
    }
    __nop();
}
