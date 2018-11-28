#ifndef __my_uart_h__
#define __my_uart_h__

#ifdef __cplusplus
extern "C"{
#endif

#define EUSART_TX_BUFFER_SIZE 32
#define EUSART_RX_BUFFER_SIZE 32
#define MIN_RX_IDLE_TIME	200

//#define RX_USED_MODEBUS 1
    
//extern volatile uint8_t eusartTxBuffer[EUSART_TX_BUFFER_SIZE];
//extern volatile uint8_t eusartTxLen;
extern volatile uint8_t eusartRxIdleTime_ms;	//used for like modbus

extern volatile uint8_t eusartRxBuffer[EUSART_RX_BUFFER_SIZE];
extern volatile uint8_t eusartRxCount;

extern void uart_send_byte(uint8_t x);
extern void uart_send_str(uint8_t* str);
extern void uart_send_len(uint8_t* buf,uint8_t len);

extern void uart_received_start(void);
extern void uart_received_process(uint8_t* buf,uint8_t len);

#ifdef __cplusplus
}
#endif
#endif