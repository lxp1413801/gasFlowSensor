#include "../../mcc_generated_files/mcc.h"
#include "../globle/globle.h"
#include "my_uart.h" 
 
//volatile uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
//volatile uint8_t eusartTxLen=0;
volatile uint8_t eusartRxIdleTime_ms=0;	//used for like modbus

volatile uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
volatile uint8_t eusartRxCount=0;

void uart_send_byte(uint8_t x)
{
	while(0 == PIR1bits.TXIF);
    
	TXREG = x;
}
#define some_nop()  do{__nop();__nop();}while(0); 

/*
void uart_send_str(uint8_t* str)
{
    //TXSTAbits.TXEN=0;
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1;  
    //while(0 == PIR1bits.TXIF);
	while(*str!='\0'){
		while(0 == PIR1bits.TXIF);
        some_nop();
		TXREG = *str;
		str++;
        //some_nop();
	}
    //while(0 == PIR1bits.TXIF);
}
*/
 uint8_t xlen;
void uart_send_len(uint8_t* buf,uint8_t len)
{
	uint8_t i;
     xlen=len;
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1;  
    //while(0 == PIR1bits.TXIF);
	for(i=0;i<xlen;i++){
		while(0 == PIR1bits.TXIF);
         //some_nop();
		TXREG = buf[i];		
        some_nop();
	}
    //while(0 == PIR1bits.TXIF);
}

void uart_received_start(void)
{
	eusartRxCount=0;
    RCSTAbits.CREN=0;
    PIE1bits.RCIE = 0;
    PIR1bits.RCIF=0;
    
    RCSTAbits.CREN=1;
	PIE1bits.RCIE = 1;
    
	eusartRxIdleTime_ms=0;
}

 void uart_received_process(uint8_t* buf,uint8_t len);