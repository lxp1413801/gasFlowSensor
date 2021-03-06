#include "../../mcc_generated_files/mcc.h"
#include "../globle/globle.h"
#include "my_uart.h" 
 
uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
volatile uint8_t eusartTxLen=0;
volatile uint16_t eusartRxIdleTime_ms=0;	//used for like modbus

volatile uint8_t _bEusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t _bEusartRxCount=0;

void uart_send_byte(uint8_t x)
{
    //TXSTAbits.TXEN=0;
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1;    
	while(0 == PIR1bits.TXIF);
	TXREG = x;
    while(0 == PIR1bits.TXIF);
}
#define some_nop()  do{__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();}while(0); 
void uart_send_str(uint8_t* str)
{
    //TXSTAbits.TXEN=0;
    //TXSTAbits.TXEN=1;    
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1; 
	while(*str!='\0'){
		while(0 == PIR1bits.TXIF);
        //while(!TX1STAbits.TRMT);
		TXREG = *str;
		str++;
        some_nop();
	}
    while(0 == PIR1bits.TXIF);
}
 
void uart_send_len(uint8_t* buf,uint8_t len)
{
	uint8_t i;
    //TXSTAbits.TXEN=0;
    //TXSTAbits.TXEN=1;    
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1; 
	for(i=0;i<len;i++){
		while(0 == PIR1bits.TXIF);
        //while(!TX1STAbits.TRMT);
		TXREG = buf[i];		
	}
    while(0 == PIR1bits.TXIF);
}

void uart_received_start(void)
{
	_bEusartRxCount=0;
	PIE1bits.RCIE = 1;
	eusartRxIdleTime_ms=0;
   // uart_received_disable();
    //uart_received_enable();
}

 void uart_received_process(uint8_t* buf,uint8_t len);