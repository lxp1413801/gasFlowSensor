#include "../includes/includes.h"
#include "../../mcc_generated_files/adc1.h"
#include "../../mcc_generated_files/mcc.h"
#include "../api/sysData.h"
#include <stdbool.h>

uint16_t rtAdcValueRsLo=0;
uint16_t rtAdcValueRsHi=0;

uint16_t rtAdcValueRcLo=0;
uint16_t rtAdcValueRcHi=0;

uint16_t rtAdcValueVoFb=0;

uint16_t resRc=10000UL;
uint16_t resRs=10000UL;

#define opaGainRcLo 3100

//继电控制测试赋值
//<<--
#define BB_RISE_Y		4500
#define BB_FALL_Y		1500

#define iDRV_PWM1_MAX	1200
#define iDRV_PWM1_MIN	5
#define __x_delta_res	2000
uint16_t		PidKp;
uint16_t		PidTi;
uint16_t		PidTd;

uint8_t pidBbSm=PID_BB_NONE;
//-->>
//p 3428
//i 3
//d 0 


#define VOUT_PWM2_MAX	1580
#define VOUT_PWM2_MIN	5
int16_t pwm2DutyForVout=VOUT_PWM2_MIN;
int16_t pwm1DutyForIdrv=iDRV_PWM1_MIN;

//uint16_t rsSimPower=0x00;
uint16_t voExpectMv;
uint16_t voExpectAdcValue=0x00;
uint16_t rsLoAvg=0;
uint16_t rsLoAvgBuf[6]={0};

uint8_t pidTestNum=0;	
int32_t bbmax=-200000L;
int32_t bbmin=200000L;
uint16_t bbt0=0,bbt1=0,bbtu;
int32_t bbta,bbu,bbku;

uint16_t pidpwm2_U=20;
//int16_t errPwm2[3];
int16_t pwm2Ei=0;

uint16_t pidU=20;
int32_t err[3];

uint16_t get_idrv_pwm1_duty(void)
{
	//return ((PWM1DCH<<8)|PWM1DCL);
    uint16_t t16=PWM1DCH;
    t16<<=8;
    t16|=PWM1DCL;
    return t16;
}

void set_idrv_pwm1_duty(uint16_t duty)
{
    
	uint16_t t16;
    pwm1DutyForIdrv=duty;
	t16=get_idrv_pwm1_duty();
	if(t16==duty)return;
    //while(!PWM1INTFbits.PRIF);
   // PWM1INTFbits.PRIF=0;
	PWM1CONbits.EN = 0;	
	PWM1DCL= (uint8_t)(duty&0xff);
	PWM1DCH=(uint8_t)(duty>>8);
	PWM1CONbits.EN = 1;	
    /*
    PIE3bits.PWM1IE = 0;    
    pwm1DutyForIdrv=duty;
    PIE3bits.PWM1IE = 1;    
     * */
}

uint16_t get_vout_pwm2_duty(void)
{
	//return ((PWM2DCH<<8)|PWM2DCL);
    uint16_t t16=PWM2DCH;
    t16<<=8;
    t16|=PWM2DCL;
    return t16;    
}

void set_vout_pwm2_duty(uint16_t duty)
{
    
	uint16_t t16;
     pwm2DutyForVout=duty;
	t16=get_vout_pwm2_duty();
    if(t16==duty)return;
    //while(!PWM2INTFbits.PRIF);
    //PWM2INTFbits.PRIF=0;   
	
	PWM2CONbits.EN = 0;	
	PWM2DCL= (uint8_t)(duty&0xff);
	PWM2DCH=(uint8_t)(duty>>8);
	PWM2CONbits.EN = 1;	
   
    /*
    PIE3bits.PWM2IE = 0;    
    pwm2DutyForVout=duty;
    PIE3bits.PWM2IE = 1;    
    */
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
/*
uint16_t get_adc_average_value(uint16_t*  buf, uint8_t len)
{
    uint8_t maxIndex,minIndex;
    uint8_t i,j;
    uint32_t ret=0;
    uint16_t max,min;
    for(j=0;j<8;j++){
        ret=0;
        max=0;
        min=0x8000;
        maxIndex=0;
        minIndex=0;
        for(i=0;i<len;i++){
            if(max<buf[i]){
                max=buf[i];
                maxIndex=i;
            }
            if(min>buf[i]){
                min=buf[i];
                minIndex=i;
            }
            ret+=buf[i];
        }
        ret=ret-max;
        ret=ret-min;
        ret=ret/(len-2);
        buf[maxIndex]=(uint16_t)ret;
        buf[minIndex]=(uint16_t)ret;
    }
    return (uint16_t)ret;
}
//uint16_t adcBuf[32];
*/
uint16_t adc_sample(uint8_t ch,uint8_t gfvr,uint8_t num)
{
	uint16_t i;
	uint16_t ret=0;
	uint16_t t16=0;
	//start fvr
	fvr_set_gain(gfvr);
	adc_init((adc_channel_t)ch);
    __delay_us(50);
	ADCON0bits.GO_nDONE = 1;
	while (ADCON0bits.GO_nDONE);	
	if(num>64)num=64;
	for(i=0;i<num;i++){
		ADCON0bits.GO_nDONE = 1;
		while (ADCON0bits.GO_nDONE);
		__nop();
		t16=ADRESH;
		t16<<=2;
		t16|=(uint16_t)(ADRESL>>6);
		//adcBuf[i]=t16;
		ret+=t16;
	}
	//ret=get_adc_average_value(adcBuf,num);
    //ret=ret*num;
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
	
	return (uint16_t)y;
	
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
	return (uint16_t)y;
}

/*
参数整定找最佳，从小到大顺序查
先是比例后积分，最后再把微分加
曲线振荡很频繁，比例度盘要放大
曲线漂浮绕大湾，比例度盘往小扳
曲线偏离回复慢，积分时间往下降
曲线波动周期长，积分时间再加长
曲线振荡频率快，先把微分降下来
*/



void pid_pwm1_idrv_b_b(void)
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
				if(pidTestNum>4){
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
			//测试的响应速度1.2,2参数比较理想
            t32=(bbku*10/14);
			PidKp=(uint16_t)t32;
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

void pid_pwm1_idrv_run(void)
{
	//int du;
	int32_t ep,ei,ed;
	int32_t t32;
	t32=((int32_t)resRs-(int32_t)resRc);
	t32=__x_delta_res-t32;
	
	err[2]=err[1];
	err[1]=err[0];
	err[0]=t32;
	

	ep=(err[0]-err[1])*PidKp;
	ei=PidKp*err[0]/PidTi;
	//ed=(err[0]-2*err[1]+err[2])*PidKp*PidTd/200;;
	//ed=0;
	t32=ep+ei;//+ed;
	t32/=10000;
	t32+=pidU;

    __nop();
	if(t32>iDRV_PWM1_MAX)t32=iDRV_PWM1_MAX;
	if(t32<iDRV_PWM1_MIN)t32=iDRV_PWM1_MIN;
	pidU=(uint16_t)t32;

	set_idrv_pwm1_duty(pidU);
}


void pid_pwm2_vout_run(void)
{
	int16_t t16,ei;
	t16=voExpectAdcValue-rtAdcValueVoFb;
	ei=t16/150;
	if(ei==0){
		if(t16>16)ei=1;
		if(t16<-16)ei=-1;
	}

	pwm2Ei=pwm2Ei+ei;
	//if(pwm2Ei>500)pwm2Ei=500;
	//if(pwm2Ei<-500)pwm2Ei=-500;
	
	t16=voExpectAdcValue/10;
	//pwm2Ei=0;
	t16=t16+pwm2Ei;
	if(t16>VOUT_PWM2_MAX)t16=VOUT_PWM2_MAX;
	if(t16<VOUT_PWM2_MIN)t16=VOUT_PWM2_MIN;
	set_vout_pwm2_duty((uint16_t)t16);
	
}

/*
void pid_pwm2_vout_run(void)
{
	//int du;
	int16_t ep,ei,ed;
	int16_t t32;
	//t32=(resRs-resRc);
	t32=voExpectAdcValue;
	t32-=rtAdcValueVoFb;
	if(t32>12000)t32=12000;
	if(t32<-12000)t32=-12000;
	
	errPwm2[2]=errPwm2[1];
	errPwm2[1]=errPwm2[0];
	errPwm2[0]=(int16_t)t32;
	
    //ep=0;
	//ep=(errPwm2[0]-errPwm2[1])/12;
    ep=(errPwm2[0]-errPwm2[1])/32;
	ei=errPwm2[0]/16;
	if(ei==0){
		if(errPwm2[0]>8)ei=1;
		if(errPwm2[0]<-8)ei=-1;
	}
	//ei=0;
	t32=pidpwm2_U+ep+ei;
	if(t32>1580)t32=1580;
	if(t32<10)t32=10;
	
	pwm2DutyForVout=(uint16_t)t32;
	set_vout_pwm2_duty(pwm2DutyForVout);
}
*/





/* 
rtAdcValueRsLoAvg：
32次最大值700mv，22378，去24000,576000000
32次最小值200mv，6394，去6400,	40960000
535040000,>>10  522500
最大输出设定1.5V,16次采样,取12000

pwm2输出采样比列调节单位负反馈，1600->12000 7.5被

*/

uint16_t calc_rs_lo_avg(uint16_t x)
{
    uint8_t i;
    uint32_t t32=0;
    
    for(i=0;i<6-1;i++){
        rsLoAvgBuf[i]=rsLoAvgBuf[i+1];
        t32+=((uint32_t)rsLoAvgBuf[i]);
    }
    rsLoAvgBuf[i]=x;
    t32+=x;
    t32/=6;
    return (uint16_t)t32;
}

uint16_t calc_expect_vout_adc_value(uint16_t x)
{
	//uint8_t index=0;
	uint16_t i;
    int32_t y0;
	int32_t y1;
	int32_t x0;
	int32_t x1;
	int32_t t32;
    
	
    for (i = 0; i < MAX_CALIB_NUM-1; i++) {
		if(sysData.calibRsAdc[i]<sysData.calibRsAdc[i+1]){
			if (x <= sysData.calibRsAdc[i+1])break;
		}else{
			//if (x >= sysData.calibRsAdc[i+1])break;
			break;
		}
    }
    //if(i)i--;
    if (i > MAX_CALIB_NUM-2)i=MAX_CALIB_NUM-2;
	y1=sysData.calibVoMV[i+1];
	y0=sysData.calibVoMV[i];

	
	x1=sysData.calibRsAdc[i+1];
	x0=sysData.calibRsAdc[i];
	
    if(x<=x0 && i==0)return (uint16_t)y0;
	if(x1<=x0)return (uint16_t)y0;
	
	//t16=y0+(x-x0)*(y1-y0)/(x1-x0);
	t32=(x-x0);
	t32=t32*(y1-y0);
	t32=t32/(x1-x0);
	t32=t32+y0;
	if(t32<0)t32=0;
	return (uint16_t)t32;
}
/*
uint16_t calc_expect_vout_adc_value(uint16_t x)
{
	//uint8_t index=0;
	uint16_t i;
    int32_t y0;
	int32_t y1;
	int32_t x0;
	int32_t x1;
	int32_t t32;
    
	
    for (i = 0; i < MAX_CALIB_NUM-1; i++) {
		if(sysData.calibRsAdc[i]<sysData.calibRsAdc[i+1]){
			if (x <= sysData.calibRsAdc[i+1])break;
		}else{
			if (x >= sysData.calibRsAdc[i+1])break;
		}
    }
    //if(i)i--;
    if (i > MAX_CALIB_NUM-2)i=MAX_CALIB_NUM-2;
	y1=sysData.calibVoMV[i+1];
	y0=sysData.calibVoMV[i];

	
	x1=sysData.calibRsAdc[i+1];
	x0=sysData.calibRsAdc[i];
	
    if(x<=x0 && i==0)return (uint16_t)y0;
	if(x1==x0)return (uint16_t)y0;
	
	//t16=y0+(x-x0)*(y1-y0)/(x1-x0);
	t32=(x-x0);
	t32=t32*(y1-y0);
	t32=t32/(x1-x0);
	t32=t32+y0;
	if(t32<0)t32=0;
	return (uint16_t)t32;
}
*/

uint16_t cal_rs_simulate_power(void)
{
    uint32_t t32;
	//uint16_t t16=rtAdcValueRsLoAvg;
    uint16_t t16=rtAdcValueRsLo;
	if(t16>24000)t16=24000;
	if(t16<6400)t16=6400;
	t32=t16;
	t32=t32*t16;
    
	t32-=40960000UL;
	t32>>=13;//此处最大1FE4,8164//65312
	if(t32>65535)t32=65535;
	t16=(uint16_t)t32;
	return t16;
	
	/*
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
	*/
}


/*
int32_t outResDiff=0x00UL;
int32_t cal_res_diff(void)
{
	int32_t x=4000;
	int32_t t32=resRs-resRc;
	x=x-t32;
	return x;
}
*/