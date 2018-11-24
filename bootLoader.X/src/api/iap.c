#include "../../mcc_generated_files/mcc.h"
#include "../globle/globle.h"
#include "my_uart.h" 
#include "../depend/m_string.h"
#include "../api/iap.h"

#define XMODEM_SOH 0x01
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18

#define APP_FLASH_START_ADDR 0x800



bool firstPackage=true;
/*
void iap_erase_app_fw(void){
    uint16_t addr=APP_FLASH_START_ADDR;
    while(addr<END_FLASH){
        FLASH_EraseBlock(addr);
        addr+=ERASE_FLASH_BLOCKSIZE;
    }
}
*/
uint8_t iap_received_package(void)
{
	uint8_t  tm=200;
	uart_received_start();
    Event &=  ~flg_EVEN_RX_RECEIVED_PKG;
    if(firstPackage){
        uart_send_byte(XMODEM_NAK);
    }
    __delay_ms(10);
	while(tm){
		if(Event &  flg_EVEN_RX_RECEIVED_PKG)break;
		__delay_ms(2);
        tm--;
	}
	if(tm)return _bEusartRxCount;
	else 
		return 0;
}
uint8_t lengthInline=0x00;
uint16_t addrInline=0x00;
uint16_t addrWritedFlash=APP_FLASH_START_ADDR;

//uint8_t *hexLineHeader;
uint8_t hexLineCharCount=0;
uint8_t hexLineBuffer[48];

uint16_t binBuffer[32+8];
uint8_t  binBufferLen=0;
//uint16_t appFlashAddr=0;
uint8_t api_write_block(uint16_t addr , uint16_t* binbuf)
{
	uint8_t i;
	uint16_t buf[32];

    FLASH_EraseBlock(addr);
	FLASH_WriteBlock(addr,binbuf);
    for (i=0; i<ERASE_FLASH_BLOCKSIZE; i++){
        buf[i] = FLASH_ReadWord((addr+i));
		if(buf[i]!=binbuf[i]){
			__nop();
			__nop();
		}
    }	
	return 0;
}


uint8_t decode_hex_len(void)
{
	uint8_t i8;
	uint16_t t16;
    __nop();
	i8=m_str_h2b(hexLineBuffer,hexLineBuffer+1,hexLineCharCount-3);
	do{
		__nop();
		if(hexLineBuffer[3]!=0x00)break;
		t16=hexLineBuffer[1];
		t16<<=8;
		t16|=hexLineBuffer[2];
		//only pic must t16>>=1;
		t16>>=1;
		addrInline=t16;
		//获取行中的flash地址
		if(addrInline==APP_FLASH_START_ADDR){
			//第一包可能有这个错误，APP区的HEX可能是正确的;
			addrInline+=0x01;
            binBuffer[binBufferLen]=0x00;
			binBufferLen++;
		}
		//获取行中的字长度
		lengthInline=hexLineBuffer[0];
		//lengthInline>>=1;//for pic16 hex format
		if(lengthInline==0)break;
		//是否跳行
		while(addrWritedFlash+binBufferLen<addrInline){
			binBuffer[binBufferLen]=0x3ff;
			binBufferLen++;
			if(binBufferLen>=ERASE_FLASH_BLOCKSIZE){
				api_write_block(addrWritedFlash,binBuffer);
                addrWritedFlash+=ERASE_FLASH_BLOCKSIZE;
                binBufferLen-=ERASE_FLASH_BLOCKSIZE;                
			}

			// if(binBufferLen){
				// m_mem_cpy_len(binBuffer,binBuffer+ERASE_FLASH_BLOCKSIZE,binBufferLen);
			// }
		}
        //获取行中的数据
		for(i8=0;i8<lengthInline;i8+=2){
			t16=hexLineBuffer[5+i8];
			t16<<=8;
			t16|=hexLineBuffer[4+i8];
			binBuffer[binBufferLen]=t16;
			binBufferLen++;	
			
			if(binBufferLen>=ERASE_FLASH_BLOCKSIZE){
				api_write_block(addrWritedFlash,binBuffer);
                addrWritedFlash+=ERASE_FLASH_BLOCKSIZE;
                binBufferLen-=ERASE_FLASH_BLOCKSIZE;	                
			}

			
		}
	}while(0);
    m_mem_set(hexLineBuffer,0,sizeof(hexLineBuffer));
    return 0;
}
uint8_t decode_xmodem_package(void)
{
    uint8_t xloc=0;
    xloc=3;
    while(xloc<_bEusartRxCount-1){
        hexLineBuffer[hexLineCharCount]=_bEusartRxBuffer[xloc];
        if(_bEusartRxBuffer[xloc]==0x0a){
            decode_hex_len();
            hexLineCharCount=0;
        }else{
            hexLineCharCount++;
        }
        xloc++;
    }
    __nop();
    return 0;
}
uint8_t iap_process(void)
{
	uint8_t ret;
    uart_send_str((uint8_t*)"send *.hex file.\r\n");
    __delay_ms(200);
    firstPackage=true;
	//初始化相关参数
	addrWritedFlash=APP_FLASH_START_ADDR;
	lengthInline=0;
	addrInline=0;
	hexLineCharCount=0;
	binBufferLen=0;
	
	while(1){
		ret=iap_received_package();
        if(ret){
            firstPackage=false;
            
            if(ret!=0x84){
                if(_bEusartRxBuffer[0]==XMODEM_EOT){
                    uart_send_byte(XMODEM_ACK);
                    break;
                }
                uart_send_byte(XMODEM_NAK);
                continue;
            }
             
            decode_xmodem_package();
            uart_send_byte(XMODEM_ACK);
        }
	}
    
    if(binBufferLen){
        if(addrWritedFlash>=APP_FLASH_START_ADDR && addrWritedFlash <END_FLASH){
            FLASH_WriteBlock(addrWritedFlash,binBuffer);
        }
    }
    uart_send_byte(XMODEM_ACK);
	return 1;
}