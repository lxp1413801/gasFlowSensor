
#define USART_MODE_MODBUS 	0
#define USART_MODE_CONSLE	1

#define USART_MODE USART_MODE_MODBUS

#define PID_CTROLLOR_EN 1

#include <stdint.h>
#include "../../mcc_generated_files/mcc.h"
#include "../includes/includes.h"
#include "../api/sysData.h"
#include "../api/modbus.h"

uint8_t str[16];

void main(void)
{
    __nop();
    isRunInBoot=0xaa;
    SYSTEM_Initialize();
    irq_global_enable();
	//set_idrv_pwm1_duty(10);
	//set_vout_pwm2_duty(10);
    TXSTAbits.TXEN=0;
    sys_data_init();
    m_mem_cpy(str,(uint8_t*)"VER1.00\r\n");
    uart_send_str(str);
   
    if(sysData.pidSetFlg0 != 0x55 || sysData.pidSetFlg1 != 0xaa){
        pidBbSm=PID_BB_NONE;
    }else{
        pidBbSm=PID_BB_EXIT;
		PidKp=sysData.pidKp;
		PidTd=sysData.pidTd;
		PidTi=sysData.pidTi;       
    }
	//uart_send_str((uint8_t*)"UUUUU");	
    while (1){
        if(Event & flg_EVEN_TICKER_100MS){
            Event &= ~flg_EVEN_TICKER_100MS;
			#if PID_CTROLLOR_EN
			rtAdcValueRsLo=adc_sample(channel_AN6,1,32);
            //rtAdcValueRsLo>>=1;
			rtAdcValueRsHi=adc_sample(channel_AN5,3,32);
            //rtAdcValueRsHi>>=1;
            
			rtAdcValueRcLo=adc_sample(channel_AN4,3,32);
            rtAdcValueRcLo>>=1;
			rtAdcValueRcHi=adc_sample(channel_AN2,3,32);
			rtAdcValueRcHi>>=1;
			
            rtAdcValueVoFb=adc_sample(channel_AN7,3,32);
            rtAdcValueVoFb>>=1;
			//此处的16次采样不能改，后面计算有依耐性
            resRc=calc_temp_rc();
			resRs=calc_temp_rs();
            
            __nop();
            if(pidBbSm>=PID_BB_EXIT){
				pid_pwm1_idrv_run();
				//rsSimPower=cal_rs_simulate_power();
                //extern uint16_t rsLoAvg;
                rsLoAvg=calc_rs_lo_avg(rtAdcValueRsLo);
                voExpectMv=calc_expect_vout_adc_value(rsLoAvg);
				voExpectAdcValue=(voExpectMv<<1);
				pid_pwm2_vout_run();
			}else{
                pid_pwm1_idrv_b_b();
				
                if(pidBbSm>=PID_BB_EXIT){
                    sysData.pidSetFlg0 = 0x55;
                    sysData.pidSetFlg1 = 0xaa;   
                    sysData.pidKp=(uint16_t)PidKp;
                    sysData.pidTd=(uint16_t)PidTd;
                    sysData.pidTi=(uint16_t)PidTi;
                    sys_data_save();
                    modbusBuf[0]=0x55;
                    modbusBuf[1]=0xaa;
                     uart_send_len(modbusBuf,2);   
                }
            }
			#endif
        }
		
		if(Event & flg_EVEN_TICKER_1000MS){
			Event &= ~flg_EVEN_TICKER_1000MS;		
			#if USART_MODE == USART_MODE_CONSLE

            m_mem_cpy(str,(uint8_t*)"Rc=");
            uart_send_str(str);
			m_int_2_str_ex(str,resRc,sizeof(str));
			uart_send_str(str);
			
			//uart_send_str((uint8_t*)"\tRs=");
            m_mem_cpy(str,(uint8_t*)"\tRs=");
            uart_send_str(str);            
			m_int_2_str_ex(str,resRs,sizeof(str));
			uart_send_str(str);			

			//uart_send_str((uint8_t*)"\ti=");
            m_mem_cpy(str,(uint8_t*)"\ti=");
            uart_send_str(str);                   
			m_int_2_str_ex(str,rtAdcValueRsLo,sizeof(str));
			uart_send_str(str);	  
            
			//uart_send_str((uint8_t*)"\texp=");
            m_mem_cpy(str,(uint8_t*)"\texp=");
            uart_send_str(str);              
			m_int_2_str_ex(str,voExpectAdcValue,sizeof(str));
			uart_send_str(str);	  
            
			//uart_send_str((uint8_t*)"\tvo=");
            m_mem_cpy(str,(uint8_t*)"\tvo=");
            uart_send_str(str);              
			m_int_2_str_ex(str,voExpectMv,sizeof(str));
			uart_send_str(str);	               
			
			//uart_send_str((uint8_t*)"\tpwm=");
            m_mem_cpy(str,(uint8_t*)"\tpwm=");
            uart_send_str(str);            
			m_int_2_str_ex(str,pwm2DutyForVout,sizeof(str));
			uart_send_str(str);	               
            
			//uart_send_str((uint8_t*)"\r\n");
            m_mem_cpy(str,(uint8_t*)"\r\n");
            uart_send_str(str);               
            #endif
		}
		#if USART_MODE == USART_MODE_MODBUS
        if(Event & flg_EVEN_RX_RECEIVED_PKG){
            Event &= ~ flg_EVEN_RX_RECEIVED_PKG;
            if(eusartRxCount>0){
                m_mem_cpy_len(modbusBuf,eusartRxBuffer,eusartRxCount);
                modbus_response_process(modbusBuf,eusartRxCount);
                
            }
            //eusartRxCount=0;
            uart_received_start();
        }
		#endif 
        __nop();
    }
    //asm("pagesel 0x1000");
    //asm("goto 0x1000");
  
}
/**
 End of File
*/