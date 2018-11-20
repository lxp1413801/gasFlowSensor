
/**
  PWM2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    pwm2.c

  @Summary
    This is the generated driver implementation file for the PWM2 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs 

  @Description
    This header file provides implementations for driver APIs for PWM2.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs  - 1.45
        Device            :  PIC16F1574
        Driver Version    :  1.0
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "pwm2.h"

/**
  Section: PWM2 APIs
*/

void PWM2_Initialize(void)
{
    // set the PWM2 to the options selected in the User Interface

     //PHIE disabled; DCIE disabled; OFIE disabled; PRIE disabled; 
    PWM2INTE = 0x01;

     //PHIF cleared; OFIF cleared; DCIF cleared; PRIF cleared; 
    PWM2INTF = 0x00;

     //PS No_Prescalar; CS FOSC; 
    PWM2CLKCON = 0x00;

     //LDS LD1_trigger; LDT disabled; LDA load; 
    PWM2LDCON = 0x00;

     //OFM independent_run; OFS OF1_match; OFO match_incrementing; 
    PWM2OFCON = 0x00;

     //PWM2PHH 0; 
    PWM2PHH = 0x00;

     //PWM2PHL 0; 
    PWM2PHL = 0x02;

     //PWM2DCH 0; 
    PWM2DCH = 0x00;

     //PWM2DCL 160; 
    PWM2DCL = 0xA0;

     //PWM2PRH 6; 
    PWM2PRH = 0x06;

     //PWM2PRL 63; 
    PWM2PRL = 0x3F;

     //PWM2OFH 0; 
    PWM2OFH = 0x00;

     //PWM2OFL 1; 
    PWM2OFL = 0x01;

     //PWM2TMRH 0; 
    PWM2TMRH = 0x00;

     //PWM2TMRL 0; 
    PWM2TMRL = 0x00;
    
     //MODE standard_PWM; POL active_lo; EN enabled; 
    PWM2CON = 0x90;
    // Clear the PWM2 interrupt flag
    //PIR3bits.PWM2IF = 0;
        
    // Enabling PWM2 interrupt.
    //PIE3bits.PWM2IE = 1;    
}    


void PWM2_Start(void)
{
    PWM2CONbits.EN = 1;		
}

void PWM2_Stop(void)
{
    PWM2CONbits.EN = 0;		
}

bool PWM2_CheckOutputStatus(void)
{
    return (PWM2CONbits.OUT);		
}

void PWM2_LoadBufferSet(void)
{
    PWM2LDCONbits.LDA = 1;		
}

void PWM2_PhaseSet(uint16_t phaseCount)
{
    PWM2PHH = (phaseCount>>8);        //writing 8 MSBs to PWMPHH register
    PWM2PHL = (phaseCount);           //writing 8 LSBs to PWMPHL register
}

void PWM2_DutyCycleSet(uint16_t dutyCycleCount)
{
    PWM2DCH = (dutyCycleCount>>8);	//writing 8 MSBs to PWMDCH register
    PWM2DCL = (dutyCycleCount);	//writing 8 LSBs to PWMDCL register		
}

void PWM2_PeriodSet(uint16_t periodCount)
{
    PWM2PRH = (periodCount>>8);	//writing 8 MSBs to PWMPRH register
    PWM2PRL = (periodCount);	//writing 8 LSBs to PWMPRL register		
}

void PWM2_OffsetSet(uint16_t offsetCount)
{
    PWM2OFH = (offsetCount>>8);	//writing 8 MSBs to PWMOFH register
    PWM2OFL = (offsetCount);	//writing 8 LSBs to PWMOFL register		
}

uint16_t PWM2_TimerCountGet(void)
{
    return ((uint16_t)((PWM2TMRH<<8) | PWM2TMRL));       		
}

bool PWM2_IsOffsetMatchOccured(void)
{
    return (PWM2INTFbits.OFIF);		
}

bool PWM2_IsPhaseMatchOccured(void)
{
    return (PWM2INTFbits.PHIF);	
}

bool PWM2_IsDutyCycleMatchOccured(void)
{
    return (PWM2INTFbits.DCIF);		
}

bool PWM2_IsPeriodMatchOccured(void)
{
    return (PWM2INTFbits.PRIF);		
}

/**
 End of File
*/


