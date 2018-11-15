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
	extern void pid_b_b_process();
	extern void pid_run_process();
	
	extern uint16_t rtAdcValueRsLoAvg;
	extern uint16_t calc_current_rs_avg();
	
	extern uint16_t pwm2DutyForVout;
	extern uint16_t cal_voout_pwm_duty(void);
	
	extern int32_t pidU;;
	extern int32_t outResDiff;
    
    extern void set_idrv_pwm1_duty(uint16_t duty);
	#ifdef	__cplusplus
		}
	#endif
#endif

