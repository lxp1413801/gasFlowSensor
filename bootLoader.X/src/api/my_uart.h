#ifndef __my_uart_h__
#define __my_uart_h__

#ifdef __cplusplus
extern "C"{
#endif

#define EUSART_TX_BUFFER_SIZE 16
#define EUSART_RX_BUFFER_SIZE 160
#define MIN_RX_IDLE_TIME	1000

    
extern  uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
extern volatile uint8_t eusartTxLen;
extern volatile uint16_t eusartRxIdleTime_ms;	//used for like modbus

extern volatile uint8_t _bEusartRxBuffer[EUSART_RX_BUFFER_SIZE];
extern volatile uint8_t _bEusartRxCount;

extern void uart_send_byte(uint8_t x);
extern void uart_send_str(uint8_t* str);
extern void uart_send_len(uint8_t* buf,uint8_t len);

extern void uart_received_start(void);
#define uart_received_disable() RCSTAbits.CREN = 0;
#define uart_received_enable() RCSTAbits.CREN = 1;
extern void uart_received_process(uint8_t* buf,uint8_t len);

#ifdef __cplusplus
}
#endif
#endif