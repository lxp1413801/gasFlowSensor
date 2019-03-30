#include "../../mcc_generated_files/mcc.h"
#include "../globle/globle.h"
#include "my_uart.h" 
#include "../depend/m_string.h"
 
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


void uart_send_str(uint8_t* str)
{
    //TXSTAbits.TXEN=0;
    uint16_t len;
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1; 
    
    if(txBufLen>0)return;
    len=m_mem_cpy(txBuf,str);
    if(len==0)return;
    while(0 == PIR1bits.TXIF);
    TXREG=txBuf[0];
    txBufLen=len;
    txBufLen--;
    txCount++;
    if(!PIE1bits.TXIE)PIE1bits.TXIE=1;

}

uint8_t xlen;
void uart_send_len(uint8_t* buf,uint8_t len)
{

    //uint16_t len;
    if(!TXSTAbits.TXEN)TXSTAbits.TXEN=1; 
    
    if(txBufLen>0)return;
    if(len>sizeof(txBuf))len=sizeof(txBuf);
    m_mem_cpy_len(txBuf,buf,len);
    if(len==0)return;
    while(0 == PIR1bits.TXIF);
    TXREG=txBuf[0];
    txBufLen=len;
    txBufLen--;
    txCount++;
    if(!PIE1bits.TXIE)PIE1bits.TXIE=1;
}

void uart_received_start(void)
{
	eusartRxCount=0;
	/*
    RCSTAbits.CREN=0;
    PIE1bits.RCIE = 0;
    PIR1bits.RCIF=0;
    
    RCSTAbits.CREN=1;
	PIE1bits.RCIE = 1;
    */
	eusartRxIdleTime_ms=0;
}

 void uart_received_process(uint8_t* buf,uint8_t len);