
#include "modbus.h"
#include "../includes/includes.h"
#include "sysData.h"
#include "../depend/crc16.h"
uint8_t modbusBuf[EUSART_RX_BUFFER_SIZE];
void modbus_response_illgeal_function(uint8_t* buf,uint8_t errcode)
{
	st_modbusErrRespStructDef* pmdbs=(st_modbusErrRespStructDef*)buf;
	if(pmdbs->addr==0x00)return;
	pmdbs->addr=sysData.id;
	if(errcode){
		pmdbs->func+=0x80;
	}
	pmdbs->errcord=errcode;
	crc_append(buf,3);
	uart_send_len(buf,5);		
}
/*
void modbus_response_write_clib_data(uint8_t* buf){	
	uint16_t startAddr=0;
	uint16_t t16;
	st_modbusWriteSingleClib* pmdbs=(st_modbusWriteSingleClib*)buf;
	if(pmdbs->addr!=sysData.id)return;
	startAddr=pmdbs->addr_hi;
	startAddr<<=8;
	startAddr |= pmdbs->addr_lo;
	//if(pmdbs->addr!=0)return;
	if(startAddr>FLOW_CALIBRATION_POINT_NUM){
		modbus_response_illgeal_function(buf,ILLEGAL_DATA_ADDRESS);
		return;		
	}
	t16=pmdbs->data_hi;
	t16<<=8;
	t16|=pmdbs->data_lo;
	*((uint16_t*)(flashOpStr.buf))=t16;
	flashOpStr.op=emFLASH_WRITE_main_CALIB_DATA;   
    if(buf[1]==FUNC_WRITE_FLASH_CLIBDATA_COVER && startAddr>=1){
        data_api_calibration_table_data_save((uint8_t)startAddr,false);
    }else{
        data_api_calibration_table_data_save((uint8_t)startAddr,true);
    }
	//modbus_response_illgeal_function(buf,0);	
    if(pmdbs->addr==0)return;
    buf[0]=mainSystemData.shortID;
    crc_append(buf,6);
    my_uart1_send_poll(buf,8);      
}


void modbus_response_read_clib_data(uint8_t* buf)
{
    uint16_t i;
	uint16_t startAddr=0;
	uint16_t len=0;
	st_modbusComReqStructDef* pmdbs=(st_modbusComReqStructDef*)buf;
	if(pmdbs->addr!=mainSystemData.shortID)return;
	startAddr=pmdbs->addr_hi;
	startAddr<<=8;
	startAddr |= pmdbs->addr_lo;
	len=pmdbs->len_hi;
	len<<=8;
	len = pmdbs->len_lo;
    if((len+startAddr)>FLOW_CALIBRATION_POINT_NUM+1 || (len>16)){
		modbus_response_illgeal_function(buf,ILLEGAL_DATA_ADDRESS);
		return;
	} 
    data_api_read_eeprom(buf+3,addr_CALIB_DATA_ADDR_EEPROM+startAddr*4,len*4);
	buf[0] = mainSystemData.shortID;
	buf[1] = 3;
	buf[2] = (uint8_t)(len*4);
	len=buf[2]+3;
    crc_append(buf,len);
    len += 2;
    my_uart1_send_poll(buf,len);
}

*/
uint16_t getRegisterVal(uint16_t addr,uint16_t *tempData)
{
	uint16_t t16 = 0; 
	uint8_t tempAddr; 
	//uint32_t TEMP;
	tempAddr = addr & 0xff;
    switch(tempAddr){
		case 0x01:*tempData = 0x5555; break;
		case 0x02:*tempData=sysData.id;break;
		case 0x03:*tempData=(sysData.pidSetFlg1<<8 | sysData.pidSetFlg0);break;
		case 0x04:*tempData=sysData.pidKp;break;
		case 0x05:*tempData=sysData.pidTi;break;
		case 0x06:*tempData=sysData.pidTd;break;
		case 0x07:*tempData=rtAdcValueRsLo;	break;
		case 0x08:*tempData=rtAdcValueRsHi;break;
		case 0x09:*tempData=rtAdcValueRcLo;break;
		case 0x0a:*tempData=rtAdcValueRcHi;break;
		case 0x0b:*tempData=resRc;break;
		case 0x0c:*tempData=resRs;break;
		case 0x0d:*tempData=rtAdcValueVoFb;break;
		case 0x0e:*tempData=voExpectMv;break;
		case 0x0f:*tempData=voExpectAdcValue;break;
		default: *tempData=0x55aa;break;
	}
	if(tempAddr>0x0f && tempAddr < 0x10+MAX_CALIB_NUM*2){
		tempAddr-=0x10;
        if(tempAddr & 0x01){
            *tempData=sysData.calibVoMV[tempAddr>>1];
        }else{
            *tempData=sysData.calibRsAdc[tempAddr>>1];
        }
    }
	return t16;
}

void modbus_response_write_single_register(uint8_t* rbuf)
{
    uint16_t startAddr=0;
	uint16_t t16,i;
	uint8_t fiSave=0;
	st_modbusComReqStructDef* pmdbs=(st_modbusComReqStructDef*)rbuf;
    if(pmdbs->addr!=sysData.id && pmdbs->addr!=0)return;
    if(sysData.id==253 && pmdbs->addr==0)return; 
	startAddr=pmdbs->addr_hi;
	startAddr<<=8;
	startAddr |= pmdbs->addr_lo;
	startAddr = startAddr & 0xff;
	if(startAddr==0x02){
		sysData.id=rbuf[5];
		fiSave=1;	
	}else if(startAddr==0x03){
		sysData.pidSetFlg1=rbuf[4];
		sysData.pidSetFlg0=rbuf[5];
		fiSave=1;	
	}else if(startAddr>0x0f && startAddr < 0x10+MAX_CALIB_NUM*2){
		startAddr-=0x10;
        if(startAddr & 0x01){
            t16=rbuf[4];
			t16<<=8;
			t16 |= rbuf[5];
			sysData.calibVoMV[startAddr>>1]=t16;	
            sysData.calibRsAdc[startAddr>>1]=rsLoAvg;
			
            if(rbuf[1]==FUNC_WRITE_SINGLE_REGISTER_EX){
                for(i=(startAddr>>1)+1;i<MAX_CALIB_NUM;i++){
                    sysData.calibVoMV[i]=t16;
                    sysData.calibRsAdc[i]=rsLoAvg;
                }
            }
            fiSave=1;
        }		
	}else{
		//modbus_response_illgeal_function(rbuf,ILLEGAL_DATA_ADDRESS);
		return;
	}
	if(fiSave)sys_data_save();
    if(pmdbs->addr==0)return;
    rbuf[0]=sysData.id;
    crc_append(rbuf,6);
    uart_send_len(rbuf,8);      
}
void modbus_response_command(uint8_t* rbuf){
    uint8_t i;
	uint16_t startAddr=0;
    uint16_t tempData,tmpAddr;
	uint8_t len=0;
	st_modbusComReqStructDef* pmdbs=(st_modbusComReqStructDef*)rbuf;
    if(pmdbs->addr!=sysData.id ||  pmdbs->addr==0)return;
	startAddr=pmdbs->addr_hi;
	startAddr<<=8;
	startAddr |= pmdbs->addr_lo;
    tmpAddr=startAddr;
	len = pmdbs->len_lo;   
    len<<=1;
    if(len>32)len=32;
    for(i=0;i<len;i=i+2,tmpAddr++){
		getRegisterVal(tmpAddr,&tempData);				
		rbuf[i+3] = tempData >> 8;				   		
		rbuf[i+4] = tempData & 0xff;			
        
    }
	rbuf[0] = sysData.id;
	rbuf[1] = 3;
	rbuf[2] = len;
	len += 3;
    crc_append(rbuf,len);
    len += 2;
    uart_send_len(rbuf,len);       
    
}
void modbus_response_process(uint8_t* rbuf,uint16_t rlen){
	uint8_t resp=0;
	st_modbusComReqStructDef* pmdbs=(st_modbusComReqStructDef*)rbuf;
	//if(!(pmdbs->addr ==0xff  || pmdbs->addr==mainSystemData.shortID || pmdbs->addr!=0x00))return;
    //if(!( pmdbs->addr==sysData.id || pmdbs->addr!=0x00))return;
    if((pmdbs->addr!=sysData.id) && (pmdbs->addr!=0x00))return;
	if(!(crc_verify(rbuf,rlen)))return ;
	//function expand expand
	switch(pmdbs->func){
        case FUNC_READ_HOLDING_REGISTERS:
            modbus_response_command(rbuf);
            break;
        case FUNC_WRITE_SINGLE_REGISTER:
        case FUNC_WRITE_SINGLE_REGISTER_EX:
             modbus_response_write_single_register(rbuf);
             break;
		/*
		case FUNC_WRITE_FLASH_SYSDATA:
			modbus_response_write_flash_sys(rbuf);
			break;	
		
		case FUNC_READ_FLASH_CLIBDATA:
			modbus_response_read_clib_data(rbuf);   
			break;				
        case    FUNC_WRITE_FLASH_CLIBDATA:
        case  FUNC_WRITE_FLASH_CLIBDATA_COVER:
			modbus_response_write_clib_data(rbuf);
			break;
		
		case FUNC_READ_FLASH_SYSDATA:
			break;
		case FUNC_READ_STATUS_REGISTER:
			modbus_response_read_status_register(rbuf);  
			break;
		case FUNC_WRITE_STATUS_REGISTER:
			modbus_response_write_status_register(rbuf);  
			break;    
		*/
		default:
			modbus_response_illgeal_function(rbuf,ILLEGAL_FUNCTION);break;		
	}	
}