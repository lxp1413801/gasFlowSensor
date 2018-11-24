#ifndef __sys_data_h__

#ifdef __cplusplus
	extern "C"{
#endif
//<<---
	//todo 

	//#define SYSDATA_START_ADDR 0xf80
    //extern const uint8_t constSysData[64] @0xf80;
	#define SYSDATA_START_ADDR 0x1f00
	/*
	#define my_flash_read_system_erase() do{ \
		FLASH_EraseBlock(SYSDATA_START_ADDR); \
		FLASH_EraseBlock(SYSDATA_START_ADDR+ERASE_FLASH_BLOCKSIZE); \
		FLASH_EraseBlock(SYSDATA_START_ADDR+ERASE_FLASH_BLOCKSIZE+ERASE_FLASH_BLOCKSIZE); \
	}while(0);
	*/

	
	#define MAX_CALIB_NUM	30
	
	typedef struct
    {
		uint8_t pidSetFlg0;
		uint8_t pidSetFlg1;
		uint8_t id;
		uint8_t reverse;
		uint16_t pidKp;
		uint16_t pidTi;
		uint16_t pidTd;
		
		uint16_t calibRsAdc[MAX_CALIB_NUM];
		uint16_t calibVoMV[MAX_CALIB_NUM];
		uint16_t  crc16;
	}sysData_t;
	extern volatile sysData_t sysData;
	
	extern uint16_t sys_data_init(void);
	extern void sys_data_save(void);


//--->>
#ifdef __cplusplus
	}
#endif

#endif
//file end
