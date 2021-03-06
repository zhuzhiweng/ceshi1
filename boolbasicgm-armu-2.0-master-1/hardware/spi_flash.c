#include "spi_flash.h"
#include "delay.h"
#include "user_config.h"
#include "GB17691A.h"
osMutexDef(spi_flash_mutex);
static osMutexId spi_flash_mutex_id;

//static uint16_t W25QXX_TYPE=0;

//SPI1速度设置函数
//SPI速度=fAPB2/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2时钟一般为84Mhz：
static void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CTRL1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CTRL1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Enable(SPI1,ENABLE); //使能SPI1
} 

static void SPI1_Init(void)
{	 
	GPIO_InitType 	GPIO_InitStructure;
	SPI_InitType 	SPI_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_SPI1, ENABLE);	
	
	GPIO_InitStructure.Pin        = GPIO_PIN_5 | GPIO_PIN_7;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_6;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA,GPIO_PIN_5|GPIO_PIN_7);
	
	SPI_InitStructure.DataDirection = SPI_DIR_DOUBLELINE_FULLDUPLEX;
    SPI_InitStructure.SpiMode       = SPI_MODE_MASTER;
    SPI_InitStructure.DataLen       = SPI_DATA_SIZE_8BITS;
    SPI_InitStructure.CLKPOL        = SPI_CLKPOL_HIGH;
    SPI_InitStructure.CLKPHA        = SPI_CLKPHA_SECOND_EDGE;
    SPI_InitStructure.NSS           = SPI_NSS_SOFT;
    SPI_InitStructure.BaudRatePres = SPI_BR_PRESCALER_256;
    SPI_InitStructure.FirstBit = SPI_FB_MSB;
    SPI_InitStructure.CRCPoly  = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

	SPI_Enable(SPI1, ENABLE); 
}  

//SPI1 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{		 			 
	while (SPI_I2S_GetStatus(SPI1, SPI_I2S_TE_FLAG) == RESET){}//等待发送区空  
	SPI_I2S_TransmitData(SPI1, TxData); //通过外设SPIx发送一个byte  数据		
	while (SPI_I2S_GetStatus(SPI1, SPI_I2S_RNE_FLAG) == RESET){} //等待接收完一个byte  
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据			    
}

//4Kbytes为一个Sector
//16个扇区为1个Block
//W25Q128
//容量为16M字节,共有128个Block,4096个Sector 
													 
  

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t W25QXX_ReadSR(uint8_t sr)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                            //使能器件   
	SPI1_ReadWriteByte(sr);    //发送读取状态寄存器命令    
	byte=SPI1_ReadWriteByte(0Xff);             //读取一个字节  
	W25QXX_CS=1;                            //取消片选     
	return byte;   
} 
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX_Write_SR(uint8_t SR, uint8_t status)   
{   
	W25QXX_CS=0;                            //使能器件   
	SPI1_ReadWriteByte(SR);   //发送写取状态寄存器命令    
	SPI1_ReadWriteByte(status);               //写入一个字节  
	W25QXX_CS=1;                            //取消片选     	      
}   
//W25QXX写使能	
//将WEL置位   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(W25X_WriteEnable);      //发送写使能  
	W25QXX_CS=1;                            //取消片选     	      
} 
//W25QXX写禁止	
//将WEL清零  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            //使能器件   
    SPI1_ReadWriteByte(W25X_WriteDisable);     //发送写禁止指令    
	W25QXX_CS=1;                            //取消片选     	      
} 
//W25QXX进入4字节地址模式
void W25QXX_EN_4ByteAddr(void)
{
	W25QXX_CS=0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_Enable4ByteAddr);     //发送进入4字节地址模式命令   
	W25QXX_CS=1;                            //取消片选     	      
}
//W25QXX读地址模式
uint8_t W25QXX_IS_4ByteAddr(void)
{
	return (W25QXX_ReadSR(W25X_ReadSR3) && 0x01);
}

//擦除整个芯片		  
//等待时间超长...
void W25QXX_Erase_Chip(void)   
{                  
                 
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS = 0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
	W25QXX_CS = 1;                            //取消片选     	      
	W25QXX_Wait_Busy();   				   //等待芯片擦除结束
}   

void W25QXX_Erase_Chip_osMutex(void)   
{                  
  osMutexWait(spi_flash_mutex_id, osWaitForever);

	W25QXX_Erase_Chip();

	osMutexRelease(spi_flash_mutex_id);
}   

//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
 //	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr *= 4096;
  W25QXX_Write_Enable();                  //SET WEL 	 
  W25QXX_Wait_Busy();   
  W25QXX_CS = 0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_SectorErase);      //发送扇区擦除指令 
#ifdef  ADDR_4BYTE_MODE
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>24));  //发送32bit地址 
#endif
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));	
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
	W25QXX_CS = 1;                            //取消片选     	      
  W25QXX_Wait_Busy();   				   //等待擦除完成
}  
//等待空闲
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(W25X_ReadSR1)&0x01) == 0x01);   // 等待BUSY位清空
}  
//进入掉电模式
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS = 0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_PowerDown);        //发送掉电命令  
	W25QXX_CS = 1;                            //取消片选     	      
  //delay_us(3);                               //等待TPD 
  for(uint16_t i=0xffff;i>0;i--);
}   
//唤醒
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS = 0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	W25QXX_CS = 1;                            //取消片选     	      
  //delay_us(3);                               //等待TRES1
	for(uint16_t i=0xffff;i>0;i--);
}
//读取芯片ID
//返回值如下:				   
//0XEF13,表示芯片型号为W25Q80  
//0XEF14,表示芯片型号为W25Q16    
//0XEF15,表示芯片型号为W25Q32  
//0XEF16,表示芯片型号为W25Q64 
//0XEF17,表示芯片型号为W25Q128 	  
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI1_ReadWriteByte(0x90);//发送读取ID命令	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	    
	SPI1_ReadWriteByte(0x00); 	 			   
	Temp|=SPI1_ReadWriteByte(0xFF)<<8;  
	Temp|=SPI1_ReadWriteByte(0xFF);	 
	W25QXX_CS=1;				    
	return Temp;
} 

void W25QXX_set_DieID(uint8_t die_id)
{
	W25QXX_CS=0;                            //使能器件   
	SPI1_ReadWriteByte(W25X_DIE_SELECT);   //
	SPI1_ReadWriteByte(die_id);               // 
	W25QXX_CS=1;                            //取消片选 
}
 
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS = 0;                            //使能器件   
  SPI1_ReadWriteByte(W25X_PageProgram);      //发送写页命令  
#ifdef  ADDR_4BYTE_MODE	
  SPI1_ReadWriteByte((u8)((WriteAddr)>>24)); //发送32bit地址
#endif	
  SPI1_ReadWriteByte((u8)((WriteAddr) >>16));	
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);   
  for(i = 0; i < NumByteToWrite; i++)SPI1_ReadWriteByte(pBuffer[i]);//循环写数  
	W25QXX_CS=1;                            //取消片选 
	W25QXX_Wait_Busy();					   //等待写入结束
} 
//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain = 256 - WriteAddr % 256; //单页剩余的字节数		 	    
	if(NumByteToWrite <= pageremain)
	{
		pageremain = NumByteToWrite;//不大于256个字节
	}
	
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite == pageremain)
		{
			break;//写入结束了
		}
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite > 256)
			{
				pageremain = 256; //一次可以写入256个字节
			}
			else 
			{
				pageremain = NumByteToWrite; 	  //不够256个字节了
			}
		}
	}	    
}
//写SPI FLASH  
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)						
//NumByteToWrite:要写入的字节数(最大65535)   

void W25QXX_Write_wo_retain(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)  
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
// 	u16 i;    
	
 	secpos = WriteAddr / FLASH_SECTOR_SIZE;//扇区地址  
	secoff = WriteAddr % FLASH_SECTOR_SIZE;//在扇区内的偏移
	secremain = FLASH_SECTOR_SIZE - secoff;//扇区剩余空间大小   
	
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite <= secremain)
	{
		secremain = NumByteToWrite;//不大于4096个字节
	}
	
	while(1) 
	{
		if(secoff==0)
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
		}		

		W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 		
		
		if(NumByteToWrite == secremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
		   secpos++;//扇区地址增1
			 secoff = 0;//偏移位置为0 
			
			//这里需要判断是否越界
			
		   pBuffer += secremain;  //指针偏移
			
			 WriteAddr += secremain;//写地址偏移
			 
		   NumByteToWrite -= secremain;				//字节数递减
			 if(NumByteToWrite > FLASH_SECTOR_SIZE)
			 {
				 secremain = FLASH_SECTOR_SIZE;	//下一个扇区还是写不完
			 }
			 else 
			 {
				 secremain = NumByteToWrite;			//下一个扇区可以写完了
			 }
		}	 
	} 
}

//读取SPI FLASH  
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
//void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{ 
	
	W25QXX_CS = 0;                            //使能器件   
	SPI1_ReadWriteByte(W25X_ReadData);         //发送读取命令
#ifdef  ADDR_4BYTE_MODE	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 24));  //发送32bit地址  
#endif	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 16));	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 8));   
	SPI1_ReadWriteByte((u8)ReadAddr);   
	for(uint16_t i = 0; i < NumByteToRead; i++)
	{ 
		pBuffer[i] = SPI1_ReadWriteByte(0XFF);   //循环读数  
	}
	W25QXX_CS = 1;		
} 
  
//初始化SPI FLASH的IO口
void W25QXX_Init(void)
{ 
	GPIO_InitType	GPIO_InitStructure;
	
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	
	GPIO_InitStructure.Pin        = GPIO_PIN_4;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);

	W25QXX_CS = 1;			//SPI FLASH不选中
	
	SPI1_Init();		   			//初始化SPI
	
	SPI1_SetSpeed(SPI_BR_PRESCALER_2);		//设置为18M时钟,高速模式 
	
//	W25QXX_TYPE = W25QXX_ReadID();	//读取FLASH ID.

#ifdef  ADDR_4BYTE_MODE
	uint8_t trycnt = 100;
	do{
	  W25QXX_EN_4ByteAddr();
		trycnt--;
	}while((!W25QXX_IS_4ByteAddr()) && trycnt);
#endif
	
	spi_flash_mutex_id = osMutexCreate (osMutex(spi_flash_mutex));

}

void read_data_from_flash(uint8_t *pdata,uint32_t read_addr,uint16_t data_len,uint8_t flash_area)
{
	
	uint16_t read_size = 0;

	uint16_t secremain;
	
	uint32_t current_read_addr;
	uint32_t flash_start_addr;
	uint32_t flash_end_addr;
	
	
	if(flash_area == ALARM_DATA_FLASH_AREA)
	{
		flash_start_addr = ALARM_DATA_START_ADDR;
		flash_end_addr = ALARM_DATA_END_ADDR;
	}
	else
	{
		if(flash_area == RESEND_DATA_FLASH_AREA)
		{
			flash_start_addr = RESEND_DATA_START_ADDR;
		  flash_end_addr = RESEND_DATA_END_ADDR;
		}
		else
		{
			if(flash_area == FIRMWARE_DATA_FLASH_AREA)
			{
			  flash_start_addr = FIRMWARE_START_ADDR;
		    flash_end_addr = FIRMWARE_END_ADDR;
			}
			else
			{
			  return;
			}
		}
	}
	
	osMutexWait (spi_flash_mutex_id, osWaitForever);
	
	if((read_addr >= flash_start_addr) && (read_addr < flash_end_addr))
	{
#ifdef W25M512
		if(read_addr >= DIE_SIZE)
		{
			if((read_addr + data_len) >= flash_end_addr)
			{
				secremain = flash_end_addr - read_addr;
			}
			else
			{
				secremain = data_len;
			}
			current_read_addr = read_addr - DIE_SIZE;
			W25QXX_set_DieID(DIE_NUM_1);
	  }
		else
		{
			if((read_addr + data_len) >= DIE_SIZE)
			{
				secremain = DIE_SIZE - read_addr;
			}
			else
			{
				secremain = data_len;
			}
			current_read_addr = read_addr;
			W25QXX_set_DieID(DIE_NUM_0);
		}
#else
		if((read_addr + data_len) >= flash_end_addr)
		{
			secremain = flash_end_addr - read_addr;
		}
		else
		{
			secremain = data_len;
		}
    current_read_addr = read_addr;
#endif		
		while(1)
		{
		  W25QXX_Read(&pdata[read_size],current_read_addr,secremain);
			read_size += secremain;
			if(read_size >= data_len)
			{
				break;
			}
			else
			{
				read_addr += secremain;
				if(read_addr >= flash_end_addr)
				{
					read_addr -= flash_end_addr;
					read_addr += flash_start_addr;
				}
				secremain = data_len - secremain;
#ifdef W25M512
				W25QXX_set_DieID(read_addr/DIE_SIZE);
				current_read_addr = read_addr % DIE_SIZE;
#else
				current_read_addr = read_addr;
#endif
			}
		}
	}
	osMutexRelease (spi_flash_mutex_id);
}

uint8_t write_data_to_flash(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite,uint8_t flash_area)
{
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;
	
	uint32_t write_addr;
	uint32_t flash_start_addr;
	uint32_t flash_end_addr;

	if(flash_area == ALARM_DATA_FLASH_AREA)
	{
		flash_start_addr = ALARM_DATA_START_ADDR;
		flash_end_addr = ALARM_DATA_END_ADDR;
	}
	else
	{
		if(flash_area == RESEND_DATA_FLASH_AREA)
		{
			flash_start_addr = RESEND_DATA_START_ADDR;
		  flash_end_addr = RESEND_DATA_END_ADDR;
		}
		else
		{
			if(flash_area == FIRMWARE_DATA_FLASH_AREA)
			{
				flash_start_addr = FIRMWARE_START_ADDR;
				flash_end_addr = FIRMWARE_END_ADDR;
			}
			else
			{
			  return 0;
			}
		}
	}
	
	osMutexWait (spi_flash_mutex_id, osWaitForever);
	
#ifdef W25M512
	W25QXX_set_DieID(WriteAddr/DIE_SIZE);
	write_addr = WriteAddr % DIE_SIZE;
#else
  write_addr = 	WriteAddr;
#endif
	
	secpos = write_addr / FLASH_SECTOR_SIZE;//扇区地址  
	secoff = write_addr % FLASH_SECTOR_SIZE;//在扇区内的偏移
	secremain = FLASH_SECTOR_SIZE - secoff;//扇区剩余空间大小   
	
 	if(NumByteToWrite <= secremain)
	{
		secremain = NumByteToWrite;//不大于4096个字节
	}
	
	while(1) 
	{
		if(secoff==0)
		{
			W25QXX_Erase_Sector(secpos);//擦除这个扇区
		}		

		W25QXX_Write_NoCheck(pBuffer,write_addr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 		
		
		if(NumByteToWrite == secremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			//secpos++;//扇区地址增1
			secoff = 0;//偏移位置为0 
			WriteAddr += secremain;//写地址偏移
			
			//这里需要判断是否越界
			if(WriteAddr >= flash_end_addr)
			{
				WriteAddr = flash_start_addr;
			}
			
#ifdef W25M512			
			W25QXX_set_DieID(WriteAddr/DIE_SIZE);
	    write_addr = WriteAddr % DIE_SIZE;
#else
			write_addr = WriteAddr;
#endif
			
			secpos = write_addr / FLASH_SECTOR_SIZE;

			pBuffer += secremain;  //指针偏移
			NumByteToWrite -= secremain;				//字节数递减
			if(NumByteToWrite > FLASH_SECTOR_SIZE)
			{
			 secremain = FLASH_SECTOR_SIZE;	//下一个扇区还是写不完
			}
			else 
			{
			 secremain = NumByteToWrite;			//下一个扇区可以写完了
			}
		}	 
	}
	osMutexRelease (spi_flash_mutex_id);
	return 1;
}



void write_resend_data_flag(uint32_t addr, uint16_t flag)
{
	uint8_t data[FLASH_PAGE_SIZE];
	uint32_t cur_page_start;
	uint32_t cur_addr;
	uint8_t page_pos;
	
	osMutexWait (spi_flash_mutex_id, osWaitForever);
	
	if((addr>>8)==((addr+1)>>8))//addr和addr+1在同一页
	{
#ifdef W25M512
		W25QXX_set_DieID((uint8_t)((addr & DIE_SIZE)>>25));
		cur_addr = addr & (DIE_SIZE - 1);
#else
		cur_addr = addr;
#endif
		cur_page_start = cur_addr & (~((uint32_t)(FLASH_PAGE_SIZE-1)));
		page_pos = (uint8_t)(cur_addr & ((uint32_t)(FLASH_PAGE_SIZE-1)));
		W25QXX_Read(data,cur_page_start,FLASH_PAGE_SIZE);
		data[page_pos] = (uint8_t)flag;
		data[page_pos+1] = (uint8_t)(flag>>8);
		W25QXX_Write_Page(data,cur_page_start,FLASH_PAGE_SIZE);
	}
	else
	{
#ifdef W25M512
		if((addr>>25)==((addr+1)>>25))//addr和addr+1在同一die
#endif
		
		{
			
#ifdef W25M512
		  W25QXX_set_DieID((uint8_t)((addr & DIE_SIZE)>>25));
			cur_addr = addr & (DIE_SIZE - 1);
#else
			cur_addr = addr;
#endif	
			cur_page_start = cur_addr & (~((uint32_t)(FLASH_PAGE_SIZE-1)));
			page_pos = (uint8_t)(cur_addr & ((uint32_t)(FLASH_PAGE_SIZE-1)));
			W25QXX_Read(data,cur_page_start,FLASH_PAGE_SIZE);
			data[page_pos] = (uint8_t)flag;
			W25QXX_Write_Page(data,cur_page_start,FLASH_PAGE_SIZE);
			
#ifdef W25M512			
			cur_addr = (addr+1) & (DIE_SIZE - 1);
#else
			cur_addr = addr + 1;
#endif
			cur_page_start = cur_addr & (~((uint32_t)(FLASH_PAGE_SIZE-1)));
			page_pos = (uint8_t)(cur_addr & ((uint32_t)(FLASH_PAGE_SIZE-1)));
			W25QXX_Read(data,cur_page_start,FLASH_PAGE_SIZE);
			data[page_pos] = (uint8_t)(flag>>8);
			W25QXX_Write_Page(data,cur_page_start,FLASH_PAGE_SIZE);
		}

#ifdef W25M512
		else
		{
			W25QXX_set_DieID((uint8_t)((addr & DIE_SIZE)>>25));
			cur_addr = addr & (DIE_SIZE - 1);
			cur_page_start = cur_addr & (~((uint32_t)(FLASH_PAGE_SIZE-1)));
			page_pos = (uint8_t)(cur_addr & ((uint32_t)(FLASH_PAGE_SIZE-1)));
			W25QXX_Read(data,cur_page_start,FLASH_PAGE_SIZE);
			data[page_pos] = (uint8_t)flag;
			W25QXX_Write_Page(data,cur_page_start,FLASH_PAGE_SIZE);
			
			W25QXX_set_DieID((uint8_t)(((addr+1) & DIE_SIZE)>>25));
			cur_addr = (addr+1) & (DIE_SIZE - 1);
			cur_page_start = cur_addr & (~((uint32_t)(FLASH_PAGE_SIZE-1)));
			page_pos = (uint8_t)(cur_addr & ((uint32_t)(FLASH_PAGE_SIZE-1)));
			W25QXX_Read(data,cur_page_start,FLASH_PAGE_SIZE);
			data[page_pos] = (uint8_t)(flag>>8);
			W25QXX_Write_Page(data,cur_page_start,FLASH_PAGE_SIZE);
		}
#endif
	}
	osMutexRelease (spi_flash_mutex_id);
}


//static void write_info_to_flash(u8* pBuffer,u16 NumByteToWrite)
//{
//	W25QXX_set_DieID(DIE_NUM_0);
//	W25QXX_Erase_Sector(INFO_SECTOR_NUM);//擦除存储信息的扇区
//	W25QXX_Write_NoCheck(pBuffer, INFO_SECTOR_NUM*FLASH_SECTOR_SIZE, NumByteToWrite);
//}


uint16_t get_lognum(rtc_time_t time)
{
	rtc_time_t read_time;
	uint8_t data[256];
	uint16_t i,j;
	uint16_t logincnt = 0;
	uint32_t read_addr = LOGIN_NUM_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
#ifdef W25M512	
	W25QXX_set_DieID(DIE_NUM_0);
#endif
	
	while(read_addr < LOGIN_NUM_END_ADDR)
	{
	  W25QXX_Read(data,read_addr,sizeof(data));
		for(i=0;i<sizeof(data);i+=8) //时间+lognum总共8个字节
		{
			if(data[i]==0xff)
			{
				for(j=0;j<8;j++) //时间+lognum总共8个字节
				{
					if(data[i+j]!=0xff)
					{
						break;
					}
				}
				
				if(j==8)
				{
				  break;
				}
			}
			else
			{
				read_time.year = data[i];
				read_time.month = data[i+1];
				read_time.date = data[i+2];
				read_time.hour = data[i+3];
				read_time.minute = data[i+4];
				read_time.second = data[i+5];
				logincnt = (uint16_t)data[i+6] + (uint16_t)(data[i+7] << 8);
			}
		}
		
		if(i!=sizeof(data))
		{
			break;
		}
		
		read_addr += sizeof(data);
  }
	
	if((read_time.year>=18)&&(read_time.month<=12)&&(read_time.date<=31)&&(read_time.hour<=24)&&(read_time.minute<60)&&(read_time.second<60))
	{
		if((read_time.date!=time.date)||(read_time.month!=time.month)||(read_time.year!=time.year))
		{
			logincnt = 0;
		}
	}
	else
	{
		logincnt = 0;
	}
	osMutexRelease(spi_flash_mutex_id);
	return logincnt;
}


void set_lognum(rtc_time_t time, uint16_t lognum)
{
	uint8_t data[256];
	uint16_t i,j;
	uint32_t write_addr = LOGIN_NUM_START_ADDR;
	uint32_t read_addr = LOGIN_NUM_START_ADDR;

	osMutexWait(spi_flash_mutex_id, osWaitForever);
#ifdef W25M512
	W25QXX_set_DieID(DIE_NUM_0);
#endif
	
	while(read_addr < LOGIN_NUM_END_ADDR)
	{
	  W25QXX_Read(data,read_addr,sizeof(data));
		for(i=0;i<sizeof(data);i+=8) //时间+lognum总共8个字节
		{
			if(data[i]==0xff)
			{
				for(j=0;j<8;j++) //时间+lognum总共8个字节
				{
					if(data[i+j]!=0xff)
					{
						break;
					}
				}
				
				if(j==8)
				{
				  break;
				}
				else
				{
					write_addr += 8;
				}
			}
			else
			{
				write_addr += 8;
			}
		}
		
		if(i!=sizeof(data))
		{
			break;
		}
		
		read_addr += sizeof(data);
  }
	
	if(write_addr >= LOGIN_NUM_END_ADDR)
	{
		write_addr = LOGIN_NUM_START_ADDR;
	}
	
	data[0] = time.year;
	data[1] = time.month;
	data[2] = time.date;
	data[3] = time.hour;
	data[4] = time.minute;
	data[5] = time.second;
	data[6] = (uint8_t)lognum;
	data[7] = (uint8_t)(lognum >> 8);
	
	if(write_addr % FLASH_SECTOR_SIZE == 0)
	{
		W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );//擦除这个扇区
	}
	W25QXX_Write_NoCheck(data, write_addr, 8);
	
	osMutexRelease(spi_flash_mutex_id);
}


/*
  SET  vin            
*/
void set_vin(uint8_t *v)
{
	uint8_t data[17];
	uint32_t write_addr = VIN_START_ADDR;
	uint32_t writebkp_addr = VINBKP_START_ADDR;	
//	uint32_t read_addr = VIN_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	//W25QXX_set_DieID(DIE_NUM_0);
	//W25QXX_Read(data,read_addr,sizeof(data));
	memcpy(&data,v,17);
	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, write_addr, sizeof(data));	
	
	W25QXX_Erase_Sector(writebkp_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, writebkp_addr, sizeof(data));		
	
	osMutexRelease(spi_flash_mutex_id);
}
 


uint8_t get_vin(uint8_t *pvin)
{
	uint8_t res = 0;
	uint8_t i=0;
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(pvin,VIN_START_ADDR,17);
	osMutexRelease(spi_flash_mutex_id);
	for(i = 0; i < 17; i++)
	{
		if(!((pvin[i] >= 0x30 && pvin[i] <= 0x39) || (pvin[i] >= 0x41 && pvin[i] <= 0x5a) || (pvin[i] >= 0x61 && pvin[i] <= 0x7a)))
		{
			break;
		}
	}
	if(i == 17)
		res = 1;
	if(res==0)
	{
		i=0;
		osMutexWait(spi_flash_mutex_id, osWaitForever);		
		W25QXX_Read(pvin,VINBKP_START_ADDR,17);
		osMutexRelease(spi_flash_mutex_id);		
		for(i = 0; i < 17; i++)
		{
			if(!((pvin[i] >= 0x30 && pvin[i] <= 0x39) || (pvin[i] >= 0x41 && pvin[i] <= 0x5a) || (pvin[i] >= 0x61 && pvin[i] <= 0x7a)))
			{
				break;
			}
		}	
		if(i == 17)
		{
		osMutexWait(spi_flash_mutex_id, osWaitForever);	
		W25QXX_Erase_Sector(VIN_START_ADDR / FLASH_SECTOR_SIZE );
		W25QXX_Write_NoCheck(pvin, VIN_START_ADDR, 17);			
		osMutexRelease(spi_flash_mutex_id);	
			
			res = 1;
		}
	}

	return res;
}
// 设置安全芯片 秘钥准备状态
void set_keystate(uint8_t state)
{
	
	uint8_t data[18];
	uint32_t write_addr = RECORD_START_ADDR;
	uint32_t read_addr = RECORD_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,read_addr,sizeof(data));
	data[16]=state;
	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, write_addr, sizeof(data));	
	
	osMutexRelease(spi_flash_mutex_id);
}
 
//读取安全芯片秘钥准备状态
uint8_t get_keystate(void)
{
	uint8_t res = 0xff;
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(&res,RECORD_START_ADDR+16,1);
	
	osMutexRelease(spi_flash_mutex_id);

	return res;
}

// 设置安全芯片ID
void set_idstate(uint8_t *id)
{
	
	uint8_t data[18];
	uint32_t write_addr = RECORD_START_ADDR;
	uint32_t read_addr = RECORD_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,read_addr,sizeof(data));
	memcpy(&data,id,16);
	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, write_addr, sizeof(data));	
	
	osMutexRelease(spi_flash_mutex_id);
}
 
//读取安全芯片ID
uint8_t get_idstate(uint8_t *readid)
{
	uint8_t res = 0xff;
	uint8_t i=0;
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(readid,RECORD_START_ADDR,16);
	osMutexRelease(spi_flash_mutex_id);
	
	if(	(readid[0]==ID_BOOL0)&&(readid[1]==ID_BOOL1)&&(readid[2]==ID_BOOL2)	)
	{
		for( i = 3; i < 16; i++)
		{
			if(!(readid[i] >= 0x30 && readid[i] <= 0x39)	)
			{
				break;
			}
		}
		if(i == 16)
		{
			res = 1;
		}
		else
			res=0;
	}
	else
			res=0;

	return res;
}


//擦除备案相关状态
void erase_record(void)
{
	uint32_t write_addr = RECORD_START_ADDR;
//	uint32_t read_addr = RECORD_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );	
	osMutexRelease(spi_flash_mutex_id);
}





// 设置车辆备案状态
void set_recordstate(uint8_t state)
{
	
	uint8_t data[18];
	uint32_t write_addr = RECORD_START_ADDR;
	uint32_t read_addr = RECORD_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,read_addr,sizeof(data));
	data[17]=state;
	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, write_addr, sizeof(data));	
	
	osMutexRelease(spi_flash_mutex_id);
}
 
//  读取车辆备案状态 
uint8_t get_recordstate(void)
{
	uint8_t res = 0xff;
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(&res,RECORD_START_ADDR+17,1);
	
	osMutexRelease(spi_flash_mutex_id);

	return res;
}



typedef struct paras
{
	u8 pro;
	u8 dcu;
	u8 compare;
	u8 mode;
	u32 ip;
	u32 port;
	//添加规避参数8个evades,每4位表示一个参数
	u32 evades;
}Paras_t;

uint32_t GetUdsPara(Para_t para)
{
	uint8_t data[20];

	uint32_t res;
	uint32_t read_addr = OBD_DNTPTC_START_ADDR;	
	osMutexWait(spi_flash_mutex_id, osWaitForever);	
//	W25QXX_set_DieID(DIE_NUM_0);
	W25QXX_Read(data,read_addr,sizeof(data));
//	res=data[para];	
	Paras_t* p=(Paras_t*)(data);
	switch(para)
	{
		case _PROTOCOL:
			res=p->pro;break;
		case _DCUID:
			res=p->dcu;break;
		case _NEED_COMPARE:
			res=p->compare;break;
		case _MODE:
			res=p->mode;break;
		case _IP:
			res=p->ip;break;
		case _PORT:
			res=p->port;break;
		case _EVADES:
			res=p->evades;break;
		default:break;
	}
		
	 osMutexRelease(spi_flash_mutex_id);
	 return res;
}

void SetUdsPara(Para_t para,u32 value)
{
	uint8_t data[20];
	uint32_t write_addr = OBD_DNTPTC_START_ADDR;
//	uint32_t read_addr = OBD_DNTPTC_START_ADDR;
	uint32_t read_addr = OBD_DNTPTC_START_ADDR;
	osMutexWait(spi_flash_mutex_id, osWaitForever);

	
	W25QXX_Read(data,read_addr,sizeof(data));
	Paras_t* p=(Paras_t*)(data);
	switch(para)
	{
		case _PROTOCOL:
			p->pro=value;break;
		case _DCUID:
			p->dcu=value;break;
		case _NEED_COMPARE:
			p->compare=value;break;
		case _MODE:
			p->mode=value;break;
		case _IP:
			p->ip=value;break;
		case _PORT:
			p->port=value;break;
		case _EVADES:
			p->evades=value;break;
		default:break;
	}	
	W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );
	W25QXX_Write_NoCheck(data, write_addr, sizeof(data));			
	osMutexRelease(spi_flash_mutex_id);
}

void	Set_evads(u8 evads_num,u8 data)
{
	u32 Evads_data;
	Evads_data=GetUdsPara(_EVADES);
	evads_num--;
	switch(evads_num)
	{
		case 0:
			Evads_data=Evads_data&0x0FFFFFFF;
			break;
		case 1:
			Evads_data=Evads_data&0xF0FFFFFF;
			break;	
		case 2:
			Evads_data=Evads_data&0xFF0FFFFF;
			break;	
		case 3:
			Evads_data=Evads_data&0xFFF0FFFF;
			break;	
		case 4:
			Evads_data=Evads_data&0xFFFF0FFF;
			break;
		case 5:
			Evads_data=Evads_data&0xFFFFF0FF;
			break;
		case 6:
			Evads_data=Evads_data&0xFFFFFF0F;
			break;
		case 7:
			Evads_data=Evads_data&0xFFFFFFF0;
			break;		
	}
	Evads_data=Evads_data|(u32)(	data	<<	((7-evads_num)*4)	);
	SetUdsPara(_EVADES,Evads_data);
}

uint8_t firmware_flash_init(void)
{
	uint8_t data[FLASH_PAGE_SIZE];
	uint32_t addr;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //读固件版本上传标志
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //清除固件信息
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE, 4); //保存固件版本上传标志。
  
	for(addr = FIRMWARE_START_ADDR;addr < FIRMWARE_END_ADDR; addr+=FLASH_SECTOR_SIZE)
	{
		W25QXX_Erase_Sector(addr / FLASH_SECTOR_SIZE );
	}
	
	osMutexRelease(spi_flash_mutex_id);
	
	return 1;
}

uint32_t get_firmware_info(char* type, uint32_t* p_version)
{
	uint32_t flag = 0;
	uint8_t data[FLASH_PAGE_SIZE];
  
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR,sizeof(data)); //读固件信息
	
	flag = (uint32_t)data[40] + (uint32_t)(data[41] << 8) + (uint32_t)(data[42] << 16) + (uint32_t)(data[43] << 24);
	
	for(uint8_t i = 0; i<strlen(__DEVICE_TYPE); i++)
	{
		type[i] = data[i];
	}
	
	//类型最大32个字节
	*p_version = (uint32_t)data[32] + (uint32_t)(data[33] << 8) + (uint32_t)(data[34] << 16) + (uint32_t)(data[35] << 24);
  
	osMutexRelease(spi_flash_mutex_id);
	return flag;
}

void set_firmware_info(char* type, uint32_t version, uint32_t fw_size, uint32_t flag)
{
	uint8_t data[FLASH_PAGE_SIZE];
	uint32_t ver_upload_flag;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //读固件版本上传标志
	ver_upload_flag = (uint32_t)data[0] + (uint32_t)(data[1] << 8) + (uint32_t)(data[2] << 16) + (uint32_t)(data[3] << 24);
	memset(data,0,32);
	for(uint8_t i = 0; i<strlen(__DEVICE_TYPE); i++)
	{
		data[i] = type[i];
	}
	data[32] = (uint8_t)version;
	data[33] = (uint8_t)(version >> 8);
	data[34] = (uint8_t)(version >> 16);
	data[35] = (uint8_t)(version >> 24);
	data[36] = (uint8_t)fw_size;
	data[37] = (uint8_t)(fw_size >> 8);
	data[38] = (uint8_t)(fw_size >> 16);
	data[39] = (uint8_t)(fw_size >> 24);
	data[40] = (uint8_t)flag;
	data[41] = (uint8_t)(flag >> 8);
	data[42] = (uint8_t)(flag >> 16);
	data[43] = (uint8_t)(flag >> 24);
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); 
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR, 44);
	data[0] = (uint8_t)ver_upload_flag;
	data[1] = (uint8_t)(ver_upload_flag >> 8);
	data[2] = (uint8_t)(ver_upload_flag >> 16);
	data[3] = (uint8_t)(ver_upload_flag >> 24);
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR+FLASH_PAGE_SIZE, 4);
	
	osMutexRelease(spi_flash_mutex_id);
}

void clear_firmware_info(void)
{
	uint8_t data[FLASH_PAGE_SIZE];
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //读固件版本上传标志
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //清除固件信息
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE, 4); //保存固件版本上传标志
	osMutexRelease(spi_flash_mutex_id);
}

uint32_t get_fw_ver_upload_flag(void)
{
	uint32_t flag = 0;
	uint8_t data[FLASH_PAGE_SIZE];
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //读固件版本上传标志
	flag = (uint32_t)data[0] + (uint32_t)(data[1] << 8) + (uint32_t)(data[2] << 16) + (uint32_t)(data[3] << 24);
	osMutexRelease(spi_flash_mutex_id);
	return flag;
}

void clear_fw_ver_upload_flag(void)
{
	uint8_t data[FLASH_PAGE_SIZE];
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR,sizeof(data)); //读固件版本信息
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //清除固件信息
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR, sizeof(data)); //保存固件版本信息
	osMutexRelease(spi_flash_mutex_id);
}


