#include "../includes/includes.h"
#include "../../mcc_generated_files/memory.h"
#include "../api/sysData.h"
#include "../depend/crc16.h"

//const uint8_t defaultBuf[256] @ 0x1f00;
volatile sysData_t sysData;

void my_flash_read_system_erase(void)
{
	/*
	FLASH_EraseBlock(SYSDATA_START_ADDR); 
	FLASH_EraseBlock(SYSDATA_START_ADDR+ERASE_FLASH_BLOCKSIZE); 
	FLASH_EraseBlock(SYSDATA_START_ADDR+ERASE_FLASH_BLOCKSIZE); 
	*/
	FLASH_EraseBlock(SYSDATA_START_ADDR); 
	FLASH_EraseBlock(SYSDATA_START_ADDR+32); 
	FLASH_EraseBlock(SYSDATA_START_ADDR+64); 
	FLASH_EraseBlock(SYSDATA_START_ADDR+96); 
    FLASH_EraseBlock(SYSDATA_START_ADDR+128); 
}

void my_flash_system_read(uint8_t* buf,uint16_t len)
{
	uint16_t flashAddr=SYSDATA_START_ADDR;
	uint16_t i;
	
    uint8_t GIEBitValue = INTCONbits.GIE;   // Save interrupt enable

    INTCONbits.GIE = 0;     // Disable interrupts
	
	for(i=0;i<len;i++){
	
		PMADRL = (flashAddr & 0x00FF);
		PMADRH = ((flashAddr & 0xFF00) >> 8);

		PMCON1bits.CFGS = 0;    // Deselect Configuration space
		PMCON1bits.RD = 1;      // Initiate Read
		NOP();
		NOP();
		buf[i]=PMDATL;
		flashAddr++;
	}
	INTCONbits.GIE = GIEBitValue;
}

uint16_t my_flash_system_write(uint8_t* buf,uint16_t len)
{
    uint16_t blocNum=0;
	uint16_t loc,j;
	uint16_t flashAddr=SYSDATA_START_ADDR;
	uint8_t GIEBitValue=INTCONbits.GIE; 
    uint8_t i=0;
    blocNum=len/ERASE_FLASH_BLOCKSIZE;
    if(len%ERASE_FLASH_BLOCKSIZE)blocNum++;
    
	INTCONbits.GIE = 0;  
    loc=0;
    for(j=0;j<blocNum;j++){
		PMCON1bits.CFGS = 0;    // Deselect Configuration space
		PMCON1bits.WREN = 1;    // Enable wrties
		PMCON1bits.LWLO = 1;    // Only load write latches  

		for(i=0;i<ERASE_FLASH_BLOCKSIZE;i++){
			
			PMADRL = (flashAddr & 0xFF);
			// Load upper 6 bits of write address
			PMADRH = ((flashAddr & 0xFF00) >> 8);

			// Load data in current address
			if(loc<len){
				PMDATL = buf[loc];
			}else{
				PMDATL = 0x00;
			}
			loc++;
			PMDATH = 0x00;
			if((i%ERASE_FLASH_BLOCKSIZE) ==  ERASE_FLASH_BLOCKSIZE -1 ){
				PMCON1bits.LWLO = 0;
			}
			
			PMCON2 = 0x55;
			PMCON2 = 0xAA;
			PMCON1bits.WR = 1;
			NOP();
			NOP();	
			flashAddr++;			
		}
		PMCON1bits.WREN = 0;       // Disable writes
    }
    INTCONbits.GIE = GIEBitValue;   // Restore interrupt enable

    return 0;	
}
void sys_data_save(void)
{
	crc_append((uint8_t*)&sysData,sizeof(sysData_t)-2);
	my_flash_read_system_erase();
	my_flash_system_write((uint8_t*)&sysData,sizeof(sysData_t));	
    PidKp=sysData.pidKp;
    PidTi=sysData.pidTi;
    PidTd=sysData.pidTd;;        
}
uint16_t sys_data_init(void)
{
	uint16_t ret,i;
	my_flash_system_read((uint8_t*)&sysData,sizeof(sysData_t));
	ret=crc_verify((uint8_t*)&sysData,sizeof(sysData_t));
	if(!ret){
		sysData.pidSetFlg0=0x55;
		sysData.pidSetFlg1=0xaa;
		sysData.id=0x01;
		for(i=0;i<MAX_CALIB_NUM;i++){
			//24000
			sysData.calibRsAdc[i]=6400+i*720;
			sysData.calibVoMV[i]=i*333;
		}
		
		sysData.pidKp=3200;
		sysData.pidTi=20;
		sysData.pidTd=5;
		
        sys_data_save();

	}
	return ret;
}



/**
 End of File
*/
