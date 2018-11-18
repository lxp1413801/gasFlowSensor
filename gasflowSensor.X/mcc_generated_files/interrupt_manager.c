/**
  Generated Interrupt Manager Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.c

  @Summary:
    This is the Interrupt Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  @Description:
    This header file provides implementations for global interrupt handling.
    For individual peripheral handlers please see the peripheral driver for
    all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC16F1574
        Driver Version    :  1.02
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.35
        MPLAB             :  MPLAB X 3.40
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

#include "interrupt_manager.h"
#include "mcc.h"
#include "../src/globle/globle.h"
#include "../src/api/my_uart.h"
uint8_t subTickerX=0x00;
uint8_t subTickerX_1s=0x00;
void interrupt INTERRUPT_InterruptManager (void)
{
    // interrupt handler
	uint8_t t8;
    if(INTCONbits.PEIE == 1 && PIE1bits.TMR2IE == 1 && PIR1bits.TMR2IF == 1)
    {
        //TMR2_ISR();
        PIR1bits.TMR2IF = 0;
        Event |= flg_EVEN_TICKER_20MS;
        subTickerX++;
		subTickerX_1s++;
        if(subTickerX>=5){
            subTickerX=0;
            Event |= flg_EVEN_TICKER_100MS;
        }
		if(subTickerX_1s>=50){
			subTickerX_1s=0;
			Event |= flg_EVEN_TICKER_1000MS;
		}
		
		if(eusartRxIdleTime_ms){
			eusartRxIdleTime_ms++;
			if(eusartRxIdleTime_ms>5){
				Event|=flg_EVEN_RX_RECEIVED_PKG;
				eusartRxIdleTime_ms=0;
			}
		}
		globalTicker++;
    }
	if(INTCONbits.PEIE == 1 && PIE1bits.RCIE == 1 && PIR1bits.RCIF == 1)
    {
        //EUSART_Receive_ISR();
		PIR1bits.RCIF=0;
		if(1 == RCSTAbits.OERR){
			RCSTAbits.CREN = 0;
			RCSTAbits.CREN = 1;
		}		
		t8=RCREG;
		if(eusartRxCount<EUSART_RX_BUFFER_SIZE){
			eusartRxBuffer[eusartRxCount]=t8;
			eusartRxCount++;
            //eusartRxIdleTime_ms=1;	
		}		
		eusartRxIdleTime_ms=1;		
    }
    else
    {
        //Unhandled Interrupt
    }
}
/**
 End of File
*/
