#ifndef __sample_h__
#define __sample_h__

	#ifdef	__cplusplus
	extern "C" {
	#endif

	extern  uint16_t rtAdcValueRsLo;
	extern uint16_t rtAdcValueRsHi;

	extern uint16_t rtAdcValueRcLo;
	extern uint16_t rtAdcValueRcHi;

	extern uint16_t rtAdcValueVoFb;

	extern uint32_t resRc;
	extern uint32_t resRs;	
	
	extern void set_vout_pwm2_duty(uint16_t duty);
	
	extern uint16_t adc_sample(uint8_t ch,uint8_t gfvr,uint8_t num);
	extern uint32_t calc_temp_rs(void);
	extern uint32_t calc_temp_rc(void);
	
	#define PID_BB_NONE     0
	#define PID_BB_INIT     1
	#define PID_BB_RISE     2
	#define PID_BB_FALL     3
	#define PID_BB_OK       4
	#define PID_BB_EXIT     5
	extern uint8_t pidBbSm;
    
    extern int32_t		PidKp;
    extern int32_t		PidTi;
    extern int32_t		PidTd;   
	extern void pid_pwm1_idrv_b_b(void);
	extern void pid_pwm1_idrv_run(void);
	extern void pid_pwm2_vout_run(void);
	
	extern uint16_t rtAdcValueRsLoAvg;
	//extern uint16_t calc_current_rs_avg();
	
	extern int16_t pwm2DutyForVout;
    extern int16_t pwm1DutyForIdrv;
	extern uint16_t cal_voout_pwm_duty(void);
	extern uint16_t calc_expect_vout_adc_value(uint16_t x);
    
	extern int32_t pidU;;
	extern int32_t outResDiff;
	
	//extern uint16_t rsSimPower;
    extern uint16_t voExpectMv;
	extern uint16_t voExpectAdcValue;
    extern uint16_t cal_rs_simulate_power(void);
    extern void set_idrv_pwm1_duty(uint16_t duty);
	
    extern uint16_t rsLoAvg;
    extern uint16_t calc_rs_lo_avg(uint16_t x);
	#ifdef	__cplusplus
		}
	#endif
#endif

