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
    extern volatile uint32_t globalTicker;
    
	//#define __20ms	16000
	//extern void delay_us(uint16_t dly);
    //extern void delay_ms(uint16_t dly);
    //event
    #define flg_EVEN_RX_RECEIVED_PKG    (1<<0)
    extern volatile uint8_t Event;    
	
    
    asm("isRunInBoot equ 0xfff");
    extern volatile unsigned char	isRunInBoot	@ 0xfff;

    

#ifdef	__cplusplus
}
#endif

#endif	/* GLOBLE_H */

