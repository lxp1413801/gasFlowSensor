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
#include "../includes/includes.h"

/*
                         Main application
 */
uint8_t str1[]="now app start!\r\n";
uint8_t ver[]="fw version 1.02!\r\n";
uint8_t str[16];

void main(void)
{
    __nop();
    isRunInBoot=0xaa;
    SYSTEM_Initialize();
    irq_global_enable();
    PWM1_Start();
	//set_idrv_pwm1_duty(480);
    while (1)
    {
        if(Event & flg_EVEN_TICKER_100MS){
            Event &= ~flg_EVEN_TICKER_100MS;
			rtAdcValueRsLo=adc_sample(channel_AN6,1,64);
            rtAdcValueRsLo>>=1;
			rtAdcValueRsHi=adc_sample(channel_AN5,3,64);
            rtAdcValueRsHi>>=1;
            
			rtAdcValueRcLo=adc_sample(channel_AN4,3,16);
			rtAdcValueRcHi=adc_sample(channel_AN2,3,16);
            
			//rtAdcValueVoFb=adc_sample(channel_AN7,3,8);
            resRc=calc_temp_rc();
			resRs=calc_temp_rs();
			rtAdcValueRsLoAvg=calc_current_rs_avg();
            __nop();
			

            if(pidBbSm>=PID_BB_EXIT){
				pid_run_process();
				pwm2DutyForVout=cal_voout_pwm_duty();
				set_vout_pwm2_duty(pwm2DutyForVout);
                
			}else{
                pid_b_b_process();
            }
            
        }
		if(Event & flg_EVEN_TICKER_1000MS){
            
			Event &= ~flg_EVEN_TICKER_1000MS;		
			uart_send_str((uint8_t*)"resRc=");
			m_int_2_str_ex(str,resRc,sizeof(str));
			uart_send_str(str);
			
			uart_send_str((uint8_t*)"\tresRs=");
			m_int_2_str_ex(str,resRs,sizeof(str));
			uart_send_str(str);			

			uart_send_str((uint8_t*)"\tiDrv=");
			m_int_2_str_ex(str,rtAdcValueRsLo,sizeof(str));
			uart_send_str(str);	  
            
			uart_send_str((uint8_t*)"\tpwm1=");
			m_int_2_str_ex(str,pidU,sizeof(str));
			uart_send_str(str);	  
            
			uart_send_str((uint8_t*)"\tpwm2=");
			m_int_2_str_ex(str,pwm2DutyForVout,sizeof(str));
			uart_send_str(str);	               
			
            
            
			uart_send_str((uint8_t*)"\r\n");
            
		}
        __nop();
    }
    //
    //
    //asm("pagesel 0x1000");
    //asm("goto 0x1000");
}
/**
 End of File
*/