#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H			    
#include "sys.h" 
#include "stdio.h"
#include <string.h>
#include "n32g45x_spi.h"
#include "n32g45x_gpio.h"
#include "RTC.h"
#include "cmsis_os.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
//#define W25Q80 	0XEF13 	
//#define W25Q16 	0XEF14
//#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
//#define W25Q128	0XEF17
//#define W25M512	0XEF18

//#define ADDR_4BYTE_MODE

#define	W25QXX_CS 		PAout(4)  		//W25QXX的片选信号

////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		  0x06 
#define W25X_WriteDisable		  0x04


#define W25X_ReadSR1		      0x05
#define W25X_ReadSR2		      0x35
#define W25X_ReadSR3		      0x15


#define W25X_WriteSR1		      0x01
#define W25X_WriteSR2		      0x31
#define W25X_WriteSR3		      0x11

#ifdef  W25M512
#define W25X_ReadData			    0x13
#else
#define W25X_ReadData			    0x03
#endif

#define W25X_FastReadData		  0x0C 
#define W25X_FastReadDual		  0x3B 

#ifdef W25M512
#define W25X_PageProgram		  0x12 
#else
#define W25X_PageProgram      0x02
#endif

#define W25X_BlockErase			  0xD8

#ifdef  W25M512
#define W25X_SectorErase		  0x21
#else
#define W25X_SectorErase		  0x20
#endif

#define W25X_ChipErase			  0xC7 
#define W25X_PowerDown			  0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			    0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 
#define W25X_Enable4ByteAddr	0xB7
#define W25X_DIE_SELECT       0xC2

#define DIE_NUM_0  (0)
#define DIE_NUM_1  (1)


#define ESSCODE_MAX_LEN  (50)

//#define FLASH_CHIP_SIZE     64    //64Mbit
//#define DIE_SIZE (0x2000000)    //256Mbit,32M byte
#define FLASH_SECTOR_SIZE   4096
#define FLASH_PAGE_SIZE     256

#define OBD_DNTPTC_SECTOR_NUM  (0)					//OBD_DNTPTC 存储区  0X0000~0X1000
#define OBD_DNTPTC_START_ADDR  (0x0000)
#define OBD_DNTPTC_END_ADDR    (0x1000)

#define LOGIN_NUM_SECTOR_NUM (1)
#define LOGIN_NUM_START_ADDR (0x1000)
#define LOGIN_NUM_END_ADDR   (0x2000)

//固件信息第一页：类型，版本，固件大小，固件更新标志
//第二页：固件版本上传标志
#define FIRMWARE_INFO_SECTOR_NUM (2)
#define FIRMWARE_INFO_START_ADDR (0x2000)
#define FIRMWARE_INFO_END_ADDR   (0x3000)

#define FIRMWARE_SECTOR_NUM (3)
#define FIRMWARE_START_ADDR (0x3000)
#define FIRMWARE_END_ADDR   (0x20000)

#define RECORDBKP_SECTOR_NUM (33)
#define RECORDBKP_START_ADDR (0x21000)
#define RECORDBKP_END_ADDR   (0x22000)

#define VINBKP_SECTOR_NUM (32)
#define VINBKP_START_ADDR (0x20000)
#define VINBKP_END_ADDR   (0x21000)

#define RESEND_DATA_SECTOR_NUM (35)
#define RESEND_DATA_START_ADDR (0x23000)   //(FLASH_SECTOR_SIZE*RESEND_DATA_SECTOR_NUM)
#define RESEND_DATA_END_ADDR   (0x180000) //(FLASH_CHIP_SIZE*1024*1024/8) //最大字节地址
#define RESEND_DATA_FLASH_SIZE (RESEND_DATA_END_ADDR-RESEND_DATA_START_ADDR)

#define ALARM_SECTOR_NUM (256)
#define ALARM_DATA_FLASH_SIZE (4*10*1024)  //40K
#define ALARM_DATA_START_ADDR (0x180000)  //(FLASH_SECTOR_SIZE*ALARM_SECTOR_NUM)
#define ALARM_DATA_END_ADDR   (0x190000)  //(ALARM_DATA_START_ADDR + ALARM_DATA_FLASH_SIZE) //最大字节地址

//空闲区域 190000-1F0000

#define VIN_SECTOR_NUM (496)
#define VIN_START_ADDR (0x1F0000)
#define VIN_END_ADDR   (0x1F1000)

#define RECORD_SECTOR_NUM (497)
#define RECORD_START_ADDR (0x1F1000)
#define RECORD_END_ADDR   (0x1F2000)


#define ALARM_DATA_FLASH_AREA 0x00
#define RESEND_DATA_FLASH_AREA 0x01
#define FIRMWARE_DATA_FLASH_AREA 0x02

typedef enum 
{
	_PROTOCOL,
	_DCUID,
	_NEED_COMPARE,
	_MODE,
//	_RD1,
//	_RD2,
//	_RD3,
//	_KEY1,
//	_KEY2,
//	_KEY3,
	_IP,	
	_PORT,
	_EVADES,
	
}Para_t;

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//读取FLASH ID
uint8_t W25QXX_ReadSR(uint8_t sr);	//读取状态寄存器 
void W25QXX_Write_SR(uint8_t SR, uint8_t status); //写状态寄存器
void W25QXX_Write_Enable(void);  		//写使能 
void W25QXX_Write_Disable(void);		//写保护
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void W25QXX_Write_wo_retain(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Erase_Chip(void);    	  	//整片擦除
void W25QXX_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void W25QXX_Wait_Busy(void);           	//等待空闲
void W25QXX_PowerDown(void);        	//进入掉电模式
void W25QXX_WAKEUP(void);				//唤醒

void W25QXX_Erase_Chip_osMutex(void);   	//整片擦除 加锁

void read_data_from_flash(uint8_t *pdata,uint32_t read_addr,uint16_t data_len,uint8_t flash_area);
uint8_t write_data_to_flash(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite,uint8_t flash_area);
//uint16_t get_lognum(uint8_t year,uint8_t month,uint8_t date);
uint16_t get_lognum(rtc_time_t time);
//void set_lognum(uint8_t year,uint8_t month,uint8_t date,uint16_t lognum);
void set_lognum(rtc_time_t time, uint16_t lognum);
uint8_t get_esscode(uint8_t *pesscode);
void clear_resend_data_flag(uint32_t addr);
void write_resend_data_flag(uint32_t addr, uint16_t flag);
uint8_t set_vin_esscode(uint8_t* pdata, uint8_t len);
void set_vin(uint8_t *v);
uint8_t get_vin(uint8_t *pvin);
uint32_t get_resend_wr_start_addr(uint32_t* record_resend_addr);
void save_resend_record_addr(uint32_t* addr, uint32_t resend_addr);


//uint8_t write_firmware(uint8_t* pdata, uint16_t len);
uint8_t firmware_flash_init(void);
uint32_t get_firmware_info(char* type, uint32_t* p_version);
void set_firmware_info(char* type, uint32_t version, uint32_t fw_size, uint32_t flag);
void clear_firmware_info(void);
uint32_t get_fw_ver_upload_flag(void);
void clear_fw_ver_upload_flag(void);

//uint8_t get_obd_DntPtc(void);
//void set_obd_DntPtc(uint8_t pObdDntPtc);

uint8_t GetUdsId(void);
void SetUdsId(uint8_t id);

//void SetUdsPara(u8 protocol,u8 dcuId,u8 compare,u8 mode );
//uint8_t GetUdsPara(u8 para);//2 compare
void Set_evads(u8 evads_num,u8 data);
uint32_t GetUdsPara(Para_t para);
void SetUdsPara(Para_t para,u32 value);

void set_keystate(uint8_t state);
uint8_t get_keystate(void);
void set_recordstate(uint8_t state);
uint8_t get_recordstate(void);
void set_idstate(uint8_t *id);
uint8_t get_idstate(uint8_t *readid);

void erase_record(void);
#endif



















