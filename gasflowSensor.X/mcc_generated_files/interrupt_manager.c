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
#include "../src/api/sample.h"
uint8_t subTickerX=0x00;
uint8_t subTickerX_1s=0x00;
uint8_t rxtemp;
uint8_t txBuf[64];
uint8_t txBufLen=0;
uint8_t txCount=0;
void interrupt INTERRUPT_InterruptManager (void)
{
    // interrupt handler
	//uint8_t t8;
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
		rxtemp=RCREG;
		if(eusartRxCount<EUSART_RX_BUFFER_SIZE){
			eusartRxBuffer[eusartRxCount]=rxtemp;
			eusartRxCount++;
            //eusartRxIdleTime_ms=1;	
		}		
		eusartRxIdleTime_ms=1;		
    }
    
    if( PIR1bits.TXIF == 1 && INTCONbits.PEIE && PIE1bits.TXIE)
    {
        //PIR1bits.TXIF=0;
        
		if(1 == RCSTAbits.OERR){
			RCSTAbits.CREN = 0;
			RCSTAbits.CREN = 1;
            RCSTAbits.SREN=0;
            RCSTAbits.SREN=1;
		}        
        if(txBufLen>0){
            TXREG=txBuf[txCount];
            txBufLen--;
            txCount++;
        }else{
            txBufLen=0;
            txCount=0;
            PIE1bits.TXIE=0;
        }
        
    }    
	
	if(PIR3bits.PWM2IF){
		PIR3bits.PWM2IF = 0;
	}
	
	if(PIR3bits.PWM1IF){
		PIR3bits.PWM1IF=0;
	}
    /*
    if(INTCONbits.PEIE == 1 && PIE3bits.PWM2IE == 1 && PIR3bits.PWM2IF == 1){
        //PWM2_ISR();
        PIR3bits.PWM2IF = 0;
        if(PWM2INTFbits.PRIF){
            PWM2INTFbits.PRIF=0;
            PWM2DCH = (uint8_t)(pwm2DutyForVout>>8);	//writing 8 MSBs to PWMPRH register
            PWM2DCL = (uint8_t)(pwm2DutyForVout & 0xff);	//writing 8 LSBs to PWMPRL register	            
        }        
    }
    if(INTCONbits.PEIE == 1 && PIE3bits.PWM1IE == 1 && PIR3bits.PWM1IF == 1){
        PIR3bits.PWM1IF = 0;
        if(PWM1INTFbits.PRIF){
            PWM1INTFbits.PRIF=0;
            PWM1DCH = (uint8_t)(pwm1DutyForIdrv>>8);	//writing 8 MSBs to PWMPRH register
            PWM1DCL =  (uint8_t)(pwm1DutyForIdrv&0xff);	//writing 8 LSBs to PWMPRL register	             
        }
    }   
*/

}
/**
 End of File
*/
