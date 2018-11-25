/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC16LF18345
        Driver Version    :  2.00
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
#include <stdint.h>
#include "../../mcc_generated_files/mcc.h"
#include "../globle/globle.h"
#include "../api/my_uart.h"
#include "../api/iap.h"

void main(void)
{
	uint8_t ret=0;
    uint16_t t16=0;
    isRunInBoot=0x55;
    isRunInBoot=isRunInBoot;
    SYSTEM_Initialize();
    irq_global_enable();
    do{
        t16=FLASH_ReadWord(0x1f00+2);
        if((t16 & 0xff) != 0xff)break;
        
        uart_received_start();
        uart_send_str((uint8_t*)"bootlaoder...\r\n");
        __delay_ms(100);
        uart_send_str((uint8_t*)"update app fw?y/n\r\n");
        __delay_ms(1000);
        t16=200;
        _bEusartRxCount=0;
        while(t16){
            __delay_ms(50);
            if(Event &  flg_EVEN_RX_RECEIVED_PKG){
                Event &=  ~flg_EVEN_RX_RECEIVED_PKG;
                if(_bEusartRxBuffer[0]== 'y' || _bEusartRxBuffer[0]== 'Y')break;
            }
            t16--;
        }
        if(t16){
            ret=iap_process();
            if(0==ret)while(1);
        }

    }while(0);
    
    //uart_send_str((uint8_t*)"boot app...\r\n");
    __nop();
    irq_global_disable();
    asm("pagesel 0x800");
    asm("goto 0x800");
}
/**
 End of File
*/