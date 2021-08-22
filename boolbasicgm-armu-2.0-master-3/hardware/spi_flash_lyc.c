#include "spi_flash.h"
#include "delay.h"
#include "user_config.h"

static osMutexDef_t  spi_flash_mutex;
static osMutexId spi_flash_mutex_id;

//static uint16_t W25QXX_TYPE=0;

//SPI1�ٶ����ú���
//SPI�ٶ�=fAPB2/��Ƶϵ��
//@ref SPI_BaudRate_Prescaler:SPI_BaudRatePrescaler_2~SPI_BaudRatePrescaler_256  
//fAPB2ʱ��һ��Ϊ84Mhz��
static void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//�ж���Ч��
	SPI1->CR1&=0XFFC7;//λ3-5���㣬�������ò�����
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); //ʹ��SPI1
} 

static void SPI1_Init(void)
{	 
  GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );//PORTAʱ��ʹ�� 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//ʹ��SPI1ʱ��
 
  //GPIOA5,6,7��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_7);
 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ�����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
 
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����	 
}  

//SPI1 ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
uint8_t SPI1_ReadWriteByte(uint8_t TxData)
{		 			 
 
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ��byte  ����
		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
 
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����			    
}

//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25Q128
//����Ϊ16M�ֽ�,����128��Block,4096��Sector 
													 
  

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t W25QXX_ReadSR(uint8_t sr)   
{  
	u8 byte=0;   
	W25QXX_CS=0;                            //ʹ������   
	SPI1_ReadWriteByte(sr);    //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI1_ReadWriteByte(0Xff);             //��ȡһ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     
	return byte;   
} 
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(uint8_t SR, uint8_t status)   
{   
	W25QXX_CS=0;                            //ʹ������   
	SPI1_ReadWriteByte(SR);   //����дȡ״̬�Ĵ�������    
	SPI1_ReadWriteByte(status);               //д��һ���ֽ�  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
}   
//W25QXXдʹ��	
//��WEL��λ   
void W25QXX_Write_Enable(void)   
{
	W25QXX_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_WriteEnable);      //����дʹ��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 
//W25QXXд��ֹ	
//��WEL����  
void W25QXX_Write_Disable(void)   
{  
	W25QXX_CS=0;                            //ʹ������   
    SPI1_ReadWriteByte(W25X_WriteDisable);     //����д��ָֹ��    
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
} 
//W25QXX����4�ֽڵ�ַģʽ
void W25QXX_EN_4ByteAddr(void)
{
	W25QXX_CS=0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_Enable4ByteAddr);     //���ͽ���4�ֽڵ�ַģʽ����   
	W25QXX_CS=1;                            //ȡ��Ƭѡ     	      
}
//W25QXX����ַģʽ
uint8_t W25QXX_IS_4ByteAddr(void)
{
	return (W25QXX_ReadSR(W25X_ReadSR3) && 0x01);
}

//��������оƬ		  
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)   
{                                   
  W25QXX_Write_Enable();                  //SET WEL 
  W25QXX_Wait_Busy();   
  W25QXX_CS = 0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	W25QXX_CS = 1;                            //ȡ��Ƭѡ     	      
	W25QXX_Wait_Busy();   				   //�ȴ�оƬ��������
}   
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
 //	printf("fe:%x\r\n",Dst_Addr);	  
 	Dst_Addr *= 4096;
  W25QXX_Write_Enable();                  //SET WEL 	 
  W25QXX_Wait_Busy();   
  W25QXX_CS = 0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_SectorErase);      //������������ָ�� 
#ifdef  ADDR_4BYTE_MODE
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>24));  //����32bit��ַ 
#endif
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>16));	
  SPI1_ReadWriteByte((u8)((Dst_Addr)>>8));   
  SPI1_ReadWriteByte((u8)Dst_Addr);  
	W25QXX_CS = 1;                            //ȡ��Ƭѡ     	      
  W25QXX_Wait_Busy();   				   //�ȴ��������
}  
//�ȴ�����
void W25QXX_Wait_Busy(void)   
{   
	while((W25QXX_ReadSR(W25X_ReadSR1)&0x01) == 0x01);   // �ȴ�BUSYλ���
}  
//�������ģʽ
void W25QXX_PowerDown(void)   
{ 
  W25QXX_CS = 0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_PowerDown);        //���͵�������  
	W25QXX_CS = 1;                            //ȡ��Ƭѡ     	      
  //delay_us(3);                               //�ȴ�TPD 
  for(uint16_t i=0xffff;i>0;i--);
}   
//����
void W25QXX_WAKEUP(void)   
{  
  W25QXX_CS = 0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB    
	W25QXX_CS = 1;                            //ȡ��Ƭѡ     	      
  //delay_us(3);                               //�ȴ�TRES1
	for(uint16_t i=0xffff;i>0;i--);
}
//��ȡоƬID
//����ֵ����:				   
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80  
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16    
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32  
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64 
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128 	  
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	W25QXX_CS=0;				    
	SPI1_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
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
	W25QXX_CS=0;                            //ʹ������   
	SPI1_ReadWriteByte(W25X_DIE_SELECT);   //
	SPI1_ReadWriteByte(die_id);               // 
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
}
 
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!	 
void W25QXX_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
 	u16 i;  
  W25QXX_Write_Enable();                  //SET WEL 
	W25QXX_CS = 0;                            //ʹ������   
  SPI1_ReadWriteByte(W25X_PageProgram);      //����дҳ����  
#ifdef  ADDR_4BYTE_MODE	
  SPI1_ReadWriteByte((u8)((WriteAddr)>>24)); //����32bit��ַ
#endif	
  SPI1_ReadWriteByte((u8)((WriteAddr) >>16));	
  SPI1_ReadWriteByte((u8)((WriteAddr)>>8));   
  SPI1_ReadWriteByte((u8)WriteAddr);   
  for(i = 0; i < NumByteToWrite; i++)SPI1_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	W25QXX_CS=1;                            //ȡ��Ƭѡ 
	W25QXX_Wait_Busy();					   //�ȴ�д�����
} 
//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite <= pageremain)
	{
		pageremain = NumByteToWrite;//������256���ֽ�
	}
	
	while(1)
	{	   
		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite == pageremain)
		{
			break;//д�������
		}
	 	else //NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite > 256)
			{
				pageremain = 256; //һ�ο���д��256���ֽ�
			}
			else 
			{
				pageremain = NumByteToWrite; 	  //����256���ֽ���
			}
		}
	}	    
}
//дSPI FLASH  
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)						
//NumByteToWrite:Ҫд����ֽ���(���65535)   

void W25QXX_Write_wo_retain(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)  
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
// 	u16 i;    
	
 	secpos = WriteAddr / FLASH_SECTOR_SIZE;//������ַ  
	secoff = WriteAddr % FLASH_SECTOR_SIZE;//�������ڵ�ƫ��
	secremain = FLASH_SECTOR_SIZE - secoff;//����ʣ��ռ��С   
	
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite <= secremain)
	{
		secremain = NumByteToWrite;//������4096���ֽ�
	}
	
	while(1) 
	{
		if(secoff==0)
		{
			W25QXX_Erase_Sector(secpos);//�����������
		}		

		W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 		
		
		if(NumByteToWrite == secremain)
		{
			break;//д�������
		}
		else//д��δ����
		{
		   secpos++;//������ַ��1
			 secoff = 0;//ƫ��λ��Ϊ0 
			
			//������Ҫ�ж��Ƿ�Խ��
			
		   pBuffer += secremain;  //ָ��ƫ��
			
			 WriteAddr += secremain;//д��ַƫ��
			 
		   NumByteToWrite -= secremain;				//�ֽ����ݼ�
			 if(NumByteToWrite > FLASH_SECTOR_SIZE)
			 {
				 secremain = FLASH_SECTOR_SIZE;	//��һ����������д����
			 }
			 else 
			 {
				 secremain = NumByteToWrite;			//��һ����������д����
			 }
		}	 
	} 
}

//��ȡSPI FLASH  
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
//void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)
{ 
	
	W25QXX_CS = 0;                            //ʹ������   
	SPI1_ReadWriteByte(W25X_ReadData);         //���Ͷ�ȡ����
#ifdef  ADDR_4BYTE_MODE	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 24));  //����32bit��ַ  
#endif	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 16));	
	SPI1_ReadWriteByte((u8)((ReadAddr) >> 8));   
	SPI1_ReadWriteByte((u8)ReadAddr);   
	for(uint16_t i = 0; i < NumByteToRead; i++)
	{ 
		pBuffer[i] = SPI1_ReadWriteByte(0XFF);   //ѭ������  
	}
	W25QXX_CS = 1;		
} 
  
//��ʼ��SPI FLASH��IO��
void W25QXX_Init(void)
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );//PORTAʱ��ʹ�� 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��

	W25QXX_CS = 1;			//SPI FLASH��ѡ��
	
	SPI1_Init();		   			//��ʼ��SPI
	
	SPI1_SetSpeed(SPI_BaudRatePrescaler_2);		//����Ϊ18Mʱ��,����ģʽ 
	
//	W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID.

#ifdef  ADDR_4BYTE_MODE
	uint8_t trycnt = 100;
	do{
	  W25QXX_EN_4ByteAddr();
		trycnt--;
	}while((!W25QXX_IS_4ByteAddr()) && trycnt);
#endif
	
	spi_flash_mutex_id = osMutexCreate (&spi_flash_mutex);
	
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
	
	secpos = write_addr / FLASH_SECTOR_SIZE;//������ַ  
	secoff = write_addr % FLASH_SECTOR_SIZE;//�������ڵ�ƫ��
	secremain = FLASH_SECTOR_SIZE - secoff;//����ʣ��ռ��С   
	
 	if(NumByteToWrite <= secremain)
	{
		secremain = NumByteToWrite;//������4096���ֽ�
	}
	
	while(1) 
	{
		if(secoff==0)
		{
			W25QXX_Erase_Sector(secpos);//�����������
		}		

		W25QXX_Write_NoCheck(pBuffer,write_addr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 		
		
		if(NumByteToWrite == secremain)
		{
			break;//д�������
		}
		else//д��δ����
		{
			//secpos++;//������ַ��1
			secoff = 0;//ƫ��λ��Ϊ0 
			WriteAddr += secremain;//д��ַƫ��
			
			//������Ҫ�ж��Ƿ�Խ��
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

			pBuffer += secremain;  //ָ��ƫ��
			NumByteToWrite -= secremain;				//�ֽ����ݼ�
			if(NumByteToWrite > FLASH_SECTOR_SIZE)
			{
			 secremain = FLASH_SECTOR_SIZE;	//��һ����������д����
			}
			else 
			{
			 secremain = NumByteToWrite;			//��һ����������д����
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
	
	if((addr>>8)==((addr+1)>>8))//addr��addr+1��ͬһҳ
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
		if((addr>>25)==((addr+1)>>25))//addr��addr+1��ͬһdie
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
//	W25QXX_Erase_Sector(INFO_SECTOR_NUM);//�����洢��Ϣ������
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
		for(i=0;i<sizeof(data);i+=8) //ʱ��+lognum�ܹ�8���ֽ�
		{
			if(data[i]==0xff)
			{
				for(j=0;j<8;j++) //ʱ��+lognum�ܹ�8���ֽ�
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
		for(i=0;i<sizeof(data);i+=8) //ʱ��+lognum�ܹ�8���ֽ�
		{
			if(data[i]==0xff)
			{
				for(j=0;j<8;j++) //ʱ��+lognum�ܹ�8���ֽ�
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
		W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );//�����������
	}
	W25QXX_Write_NoCheck(data, write_addr, 8);
	
	osMutexRelease(spi_flash_mutex_id);
}

/*
 ��FLSAH��ȡOBD���Э��,
 ����ֵ��OBD���Э��
*/
uint8_t get_obd_DntPtc(void)
{
	uint8_t data[2];
	uint16_t i,j;
	uint16_t res;
	uint32_t read_addr = OBD_DNTPTC_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_set_DieID(DIE_NUM_0);
	W25QXX_Read(data,read_addr,sizeof(data));
	read_addr +=sizeof(data);
	while(read_addr < OBD_DNTPTC_END_ADDR)
	{
	  res=data[1];
		W25QXX_Read(data,read_addr,sizeof(data));
		if(data[0]==0xaa)
		{
			res=data[1];
			read_addr += sizeof(data);
			if(read_addr >=OBD_DNTPTC_END_ADDR)
			{	
        osMutexRelease(spi_flash_mutex_id);				
				return res;
			}
		  W25QXX_Read(data,read_addr,sizeof(data));
			if(data[0]==0xaa){
				read_addr += sizeof(data);
			  break;
			}
			else
			{
         osMutexRelease(spi_flash_mutex_id);				
			   return res;	
			}
		}else{
     osMutexRelease(spi_flash_mutex_id);			
		 return res;
		}	
	}
	 osMutexRelease(spi_flash_mutex_id);
	 return res;
	
}

/*
  FLASH�洢OBD���Э��pObdDntPtc, �洢���ֽ�0xaa �����Э��,  0xaa ��Ϊһ��֡ͷ
*/
void set_obd_DntPtc(uint8_t pObdDntPtc)
{
	uint8_t data[2];
	uint32_t write_addr = OBD_DNTPTC_START_ADDR;
	uint32_t read_addr = OBD_DNTPTC_START_ADDR;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_set_DieID(DIE_NUM_0);
	while(read_addr < OBD_DNTPTC_END_ADDR)
	{
	  W25QXX_Read(data,read_addr,sizeof(data));
		if(data[0]==0xaa)
		{
			write_addr += sizeof(data);
		}else{
		  break;
		}	
		read_addr += sizeof(data);	
  }
	
	if(write_addr >= OBD_DNTPTC_END_ADDR)
	{
		write_addr = OBD_DNTPTC_START_ADDR;
	}
	
	data[0] = 0xaa;
	data[1] = pObdDntPtc;
	
	if(write_addr % FLASH_SECTOR_SIZE == 0)
	{
		W25QXX_Erase_Sector(write_addr / FLASH_SECTOR_SIZE );//??????
	}
	W25QXX_Write_NoCheck(data, write_addr, 2);
	
	osMutexRelease(spi_flash_mutex_id);
}


uint8_t firmware_flash_init(void)
{
	uint8_t data[FLASH_PAGE_SIZE];
	uint32_t addr;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //���̼��汾�ϴ���־
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //����̼���Ϣ
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE, 4); //����̼��汾�ϴ���־��
  
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
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR,sizeof(data)); //���̼���Ϣ
	
	flag = (uint32_t)data[40] + (uint32_t)(data[41] << 8) + (uint32_t)(data[42] << 16) + (uint32_t)(data[43] << 24);
	
	for(uint8_t i = 0; i<strlen(__DEVICE_TYPE); i++)
	{
		type[i] = data[i];
	}
	
	//�������32���ֽ�
	*p_version = (uint32_t)data[32] + (uint32_t)(data[33] << 8) + (uint32_t)(data[34] << 16) + (uint32_t)(data[35] << 24);
  
	osMutexRelease(spi_flash_mutex_id);
	return flag;
}

void set_firmware_info(char* type, uint32_t version, uint32_t fw_size, uint32_t flag)
{
	uint8_t data[FLASH_PAGE_SIZE];
	uint32_t ver_upload_flag;
	
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //���̼��汾�ϴ���־
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
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //���̼��汾�ϴ���־
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //����̼���Ϣ
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE, 4); //����̼��汾�ϴ���־
	osMutexRelease(spi_flash_mutex_id);
}

uint32_t get_fw_ver_upload_flag(void)
{
	uint32_t flag = 0;
	uint8_t data[FLASH_PAGE_SIZE];
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR + FLASH_PAGE_SIZE,sizeof(data)); //���̼��汾�ϴ���־
	flag = (uint32_t)data[0] + (uint32_t)(data[1] << 8) + (uint32_t)(data[2] << 16) + (uint32_t)(data[3] << 24);
	osMutexRelease(spi_flash_mutex_id);
	return flag;
}

void clear_fw_ver_upload_flag(void)
{
	uint8_t data[FLASH_PAGE_SIZE];
	osMutexWait(spi_flash_mutex_id, osWaitForever);
	W25QXX_Read(data,FIRMWARE_INFO_START_ADDR,sizeof(data)); //���̼��汾��Ϣ
	W25QXX_Erase_Sector(FIRMWARE_INFO_START_ADDR / FLASH_SECTOR_SIZE ); //����̼���Ϣ
	W25QXX_Write_NoCheck(data, FIRMWARE_INFO_START_ADDR, sizeof(data)); //����̼��汾��Ϣ
	osMutexRelease(spi_flash_mutex_id);
}



//uint8_t get_esscode(uint8_t *pesscode)
//{
//	uint8_t i=0;
//	W25QXX_set_DieID(DIE_NUM_0);
//	W25QXX_Read(pesscode,SET_ESSCODE_ADDR,ESSCODE_MAX_LEN); //���ܱ����50�ֽ�
//	for(i=0;i<ESSCODE_MAX_LEN;i++)
//	{
//		if(pesscode[i]<0x30 || pesscode[i]>0x39)//���ܱ�����0-9��ACSII��
//		{
//			break;
//		}
//	}
//	return i;
//}

//uint8_t get_vin(uint8_t *pvin)
//{
//	uint8_t res = 0;
//	uint8_t i=0;
//	W25QXX_set_DieID(DIE_NUM_0);
//	W25QXX_Read(pvin,SET_VIN_ADDR,17);
//	for(i = 0; i < 17; i++)
//	{
//		if(!((pvin[i] >= 0x30 && pvin[i] <= 0x39) || (pvin[i] >= 0x41 && pvin[i] <= 0x5a) || (pvin[i] >= 0x61 && pvin[i] <= 0x7a)))
//		{
//			break;
//		}
//	}
//	if(i == 17)
//	{
//		res = 1;
//	}
//	return res;
//}

//uint8_t set_vin_esscode(uint8_t* pdata, uint8_t len)
//{
//	uint8_t i;
//	uint8_t save_info[72];
//	uint8_t res = 0;
//	W25QXX_set_DieID(DIE_NUM_0);
//	W25QXX_Read(save_info,SET_VIN_ADDR,sizeof(save_info));
//  for(i = 0; i < len; i++)
//	{
//		save_info[i] = pdata[i];
//	}
//	for(;i < 67;i++)
//	{
//		save_info[i] = 0xff;
//	}
//  write_info_to_flash(save_info,sizeof(save_info));
//	
//	W25QXX_Read(save_info,SET_VIN_ADDR,sizeof(save_info));
//	for(i = 0; i < len; i++)
//	{
//		if(save_info[i] != pdata[i])
//		{
//			break;
//		}
//	}
//	if(i  == len)
//	{
//		res = 1;
//	}
//	return res;
//}

//uint32_t get_resend_wr_start_addr(uint32_t* record_addr)
//{
//	uint8_t read_data[FLASH_PAGE_SIZE];
//	uint32_t ckeck_data;
//	uint16_t i;
//	uint32_t resend_start_addr = RESEND_DATA_START_ADDR;
//  uint32_t addr = RESEND_RECORD_START_ADDR;
//	
//	*record_addr = RESEND_RECORD_START_ADDR;
//	
//	W25QXX_set_DieID(DIE_NUM_0);
//	
//	while(addr < RESEND_RECORD_END_ADDR)
//	{
//		W25QXX_Read(read_data,addr,sizeof(read_data));
//		for(i=0;i<sizeof(read_data);i+=4)
//		{
//			ckeck_data = (uint32_t)(read_data[i+3] << 24) + (uint32_t)(read_data[i+2] << 16) + (uint32_t)(read_data[i+1] << 8) + (uint32_t)read_data[i];
//			if(ckeck_data == 0xffffffff)
//			{
//				break;
//			}
//			else
//			{
//				resend_start_addr = ckeck_data;
//				(*record_addr) += 4;
//			}
//		}
//		if(i!=sizeof(read_data))
//		{
//			break;
//		}
//		addr += sizeof(read_data);
//	}
//	
//	if((resend_start_addr >= RESEND_DATA_START_ADDR) && (resend_start_addr < RESEND_DATA_END_ADDR))
//	{
//		return resend_start_addr;
//	}
//	else
//	{
//    return RESEND_DATA_START_ADDR;
//	}
//}

//void save_resend_record_addr(uint32_t* addr, uint32_t resend_addr)
//{
//	uint8_t data[4];
//	if(((*addr) >= RESEND_RECORD_START_ADDR) && ((*addr) < RESEND_RECORD_END_ADDR))
//	{
//		data[0] = (uint8_t)resend_addr;
//		data[1] = (uint8_t)(resend_addr >> 8);
//		data[2] = (uint8_t)(resend_addr >> 16);
//		data[3] = (uint8_t)(resend_addr >> 24);
//		W25QXX_set_DieID(DIE_NUM_0);
//		if((*addr) % FLASH_SECTOR_SIZE == 0)
//		{
//			W25QXX_Erase_Sector((*addr) / FLASH_SECTOR_SIZE );//�����������
//		}
//		W25QXX_Write_NoCheck(data, *addr, sizeof(data));
//		
//		(*addr) += 4;
//		if((*addr) >= RESEND_RECORD_END_ADDR)
//		{
//			(*addr) = RESEND_RECORD_START_ADDR;
//		}
//  }
//	else
//	{
//		(*addr) = RESEND_RECORD_START_ADDR;
//	}
//	
//}
