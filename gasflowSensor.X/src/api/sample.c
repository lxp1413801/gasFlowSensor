#include "../includes/includes.h"
#include "../../mcc_generated_files/adc1.h"
#include "../../mcc_generated_files/mcc.h"
#include <stdbool.h>

uint16_t rtAdcValueRsLo=0;
uint16_t rtAdcValueRsHi=0;

uint16_t rtAdcValueRcLo=0;
uint16_t rtAdcValueRcHi=0;

uint16_t rtAdcValueVoFb=0;

uint32_t resRc=10000UL;
uint32_t resRs=10000UL;

#define opaGainRcLo 3140

uint16_t get_idrv_pwm1_duty(void)
{
	return ((PWM1DCH<<8)|PWM1DCL);
}

void set_idrv_pwm1_duty(uint16_t duty)
{
	uint16_t t16;
	t16=get_idrv_pwm1_duty();
	if(t16==duty)return;
	PWM1CONbits.EN = 0;	
	PWM1DCL= (uint8_t)(duty&0xff);
	PWM1DCH=(uint8_t)(duty>>8);
	PWM1CONbits.EN = 1;	
}

uint16_t get_vout_pwm2_duty(void)
{
	return ((PWM2DCH<<8)|PWM2DCL);
}

void set_vout_pwm2_duty(uint16_t duty)
{
	uint16_t t16;
	t16=get_vout_pwm2_duty();
	if(t16==duty)return;
	PWM2CONbits.EN = 0;	
	PWM2DCL= (uint8_t)(duty&0xff);
	PWM2DCH=(uint8_t)(duty>>8);
	PWM2CONbits.EN = 1;	
}


void fvr_set_gain(uint8_t g)
{
	uint8_t t8=0x80;
	if(g>3)g=3;
	t8|=g;
	FVRCON = t8;
	while(!(FVRCONbits.FVRRDY));
}

void adc_init(adc_channel_t ch)
{
	while (ADCON0bits.GO_nDONE);
    ADCON0bits.CHS = ch;    
    ADCON0bits.ADON = 1;  	
    ADCON1 = 0x63;
    ADCON2 = 0x00;
    ADRESL = 0x00;
    ADRESH = 0x00;	
}



uint16_t adc_sample(uint8_t ch,uint8_t gfvr,uint8_t num)
{
	uint8_t i;
	uint16_t ret=0;
	uint16_t t16=0;
	//start fvr
	fvr_set_gain(gfvr);
	adc_init((adc_channel_t)ch);
    __delay_us(10);
	if(num>64)num=64;
	for(i=0;i<num;i++){
		ADCON0bits.GO_nDONE = 1;
		while (ADCON0bits.GO_nDONE);
		__nop();
		t16=ADRESH;
		t16<<=2;
		t16|=(ADRESL>>6);
		ret+=t16;
	}
	__nop;
	return ret;	
}


uint32_t calc_temp_rs(void)
{
	/*
	float x,y;
	y=120.0;
	y=y*(float)rtAdcValueRsHi;
	x=(float)rtAdcValueRsLo;
	
	y=y/x;
	if(y<50)y=50;
	if(y>200)y=200;
	return y;
	*/
	uint32_t x,y;
	y=12000UL;
	y=y*rtAdcValueRsHi;
	x=rtAdcValueRsLo;
	y=y/x;
	if(y<2000)y=2000;
	if(y>25000)y=25000;	
	y=y-1000;
	
	return y;
	
}

uint32_t calc_temp_rc(void)
{

	uint32_t x,y;	
	x=300000UL;//3K
	x=x*rtAdcValueRcLo;
	
	
	y=rtAdcValueRcHi;
	y=y*opaGainRcLo;
	y=y/100;
	y=y-rtAdcValueRcLo;
	
	
	y=x/y;
	if(y<2000)y=2000;
	if(y>25000)y=25000;	
	//y=y-1000;	
	return y;
}



uint8_t pidBbSm=PID_BB_NONE;
uint8_t pidTestNum=0;	

//继电控制测试赋值

#define BB_RISE_Y		3500
#define BB_FALL_Y		500

#define iDRV_PWM1_MAX	1200
#define iDRV_PWM1_MIN	2
#define __x_delta_res	2000



int32_t		PidKp;
int32_t		PidTi;
int32_t		PidTd;
bool	flgPidSetOK=false;
int32_t bbmax=-200000L;
int32_t bbmin=200000L;
uint32_t bbt0=0,bbt1=0;
int32_t bbtu,bbta,bbu,bbku;
void pid_b_b_process(void)
{
	int32_t t32;
	t32=(resRs-resRc);
	switch(pidBbSm){
		case PID_BB_EXIT:return;
		case PID_BB_NONE:
		case PID_BB_INIT:
			pidTestNum=0;
			set_idrv_pwm1_duty(iDRV_PWM1_MAX);
			pidBbSm=PID_BB_RISE;
            break;
		case PID_BB_RISE:
			if(t32>BB_RISE_Y){
				set_idrv_pwm1_duty(iDRV_PWM1_MIN);
				pidBbSm=PID_BB_FALL;
				break;				
			}
            break;	
		case PID_BB_FALL:
			if(t32<BB_FALL_Y){
				bbt0=bbt1;
				bbt1=globalTicker;
				pidTestNum++;
				if(pidTestNum>5){
					pidBbSm=PID_BB_OK;
					break;
				}
				set_idrv_pwm1_duty(iDRV_PWM1_MAX);
				pidBbSm=PID_BB_RISE;
				break;				
			}
            break;	
		case PID_BB_OK:
			bbtu=bbt1-bbt0;
			bbtu=bbtu/5;
			bbta=(bbmax-bbmin)/2;
			bbu=(iDRV_PWM1_MAX-iDRV_PWM1_MIN)/2;
			//bbku=314*bbta/(bbu*4);
			bbku=(bbu*40000)/(bbta*314);
            bbku*=100;
			//
			PidKp=bbku*10/17;
			PidTi=bbtu*10/20;
			PidTd=bbtu/8;			
			pidBbSm=PID_BB_EXIT;
            break;	
	}
	if(pidTestNum>2){
		if(bbmax<t32)bbmax=t32;
		if(bbmin>t32)bbmin=t32;
	}
	
}
int32_t pidU=20;
int32_t err[3];
void pid_run_process(void)
{
	//int du;
	int32_t ep,ei,ed;
	int32_t t32;
	t32=(resRs-resRc);
	t32=__x_delta_res-t32;
	
	err[2]=err[1];
	err[1]=err[0];
	err[0]=t32;
	

	ep=(err[0]-err[1])*PidKp;
	ei=PidKp*err[0]/PidTi;
	//ed=(err[0]-2*err[1]+err[2]);
    //ed=ed*PidKp*PidTd;
	ed=0;
	t32=ep+ei+ed;
	t32/=10000;
	t32+=pidU;

    __nop();
	if(t32>iDRV_PWM1_MAX)t32=iDRV_PWM1_MAX;
	if(t32<iDRV_PWM1_MIN)t32=iDRV_PWM1_MIN;
	pidU=t32;
    uint16_t t16;
    t16=(uint16_t)pidU;
	set_idrv_pwm1_duty(t16);
}
uint16_t rsCrrentBuf[8]={0};
uint16_t rtAdcValueRsLoAvg=0;
uint16_t calc_current_rs_avg(void)
{
	/*
	uint8_t i;
	uint32_t t32=0;
	for(i=0;i<7;i++){
		rsCrrentBuf[i]=rsCrrentBuf[i+1];
        t32+=rsCrrentBuf[i];
	}
	rsCrrentBuf[7]=rtAdcValueRsLo;
	t32+=rtAdcValueRsLo;
	t32/=8;
	//rtAdcValueRsLoAvg=(uint16_t)t32;
    if(t32>30000)t32=30000;
	return (uint16_t)t32;
	*/
	return rtAdcValueRsLo;
}

#define VOUT_PWM2_MAX	1580
#define VOUT_PWM2_MIN	20

uint16_t pwm2DutyForVout=0x10;
uint16_t cal_voout_pwm_duty(void)
{
    uint32_t t32;
    t32=rtAdcValueRsLoAvg;
    if(t32>7200){
        t32-=7200;
    }else{
     t32=0;
    }
    //t32-=8000;
    t32*=1600;
    t32/=13000;
    if(t32>VOUT_PWM2_MAX)t32=VOUT_PWM2_MAX;
    if(t32<VOUT_PWM2_MIN)t32=VOUT_PWM2_MIN;
    return (uint16_t)t32;
}
int32_t outResDiff=0x00UL;
int32_t cal_res_diff(void)
{
	int32_t x=4000;
	int32_t t32=resRs-resRc;
	x=x-t32;
	return x;
}