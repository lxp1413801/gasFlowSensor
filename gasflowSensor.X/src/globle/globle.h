/* 
 * File:   globle.h
 * Author: Administrator
 *
 * Created on 2017?7?16?, ??8:03
 */

#ifndef GLOBLE_H
#define	GLOBLE_H

#ifdef	__cplusplus
extern "C" {
#endif
    #include <stdint.h>
	#define irq_global_enable() do{ \
		INTERRUPT_GlobalInterruptEnable(); \
		INTERRUPT_PeripheralInterruptEnable();\
	}while(0);
	
	#define irq_global_disable() do{ \
		INTERRUPT_GlobalInterruptDisable(); \
		INTERRUPT_PeripheralInterruptDisable();\
	}while(0);
    extern volatile uint16_t globalTicker;
    
	#define __20ms	5020
	//extern void delay(uint16_t dly);
    //extern void delay_ms(uint16_t dly);
    //event
    #define flg_EVEN_RX_RECEIVED_PKG    (1<<0)
	#define flg_EVEN_TICKER_20MS		(1<<1)
	#define flg_EVEN_TICKER_100MS		(1<<2)
	#define flg_EVEN_TICKER_1000MS		(1<<3)
    extern volatile uint8_t Event;    
	
    
    asm("isRunInBoot equ 0xfff");
    extern volatile unsigned char	isRunInBoot	@ 0xfff;

    

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBLE_H */

