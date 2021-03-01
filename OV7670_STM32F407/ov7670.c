#include "ov7670.h"

uint32_t length = 160*120*2;
volatile uint8_t resim[144*176*2];

const uint8_t OV7670_reg[156][2] = {
/*
 * Clock scale: 3 = 15fps
 *              2 = 20fps
 *              1 = 30fps
 */
	{ REG_CLKRC, 0x1 },	/* OV: clock scale (30 fps) */
	{ REG_TSLB,  0x04 },	/* OV */
	{ REG_COM7, 0 },	/* VGA */
	/*
	 * Set the hardware window.  These values from OV don't entirely
	 * make sense - hstop is less than hstart.  But they work...
	 */
	{ REG_HSTART, 0x13 },	{ REG_HSTOP, 0x01 },
	{ REG_HREF, 0xb6 },	{ REG_VSTART, 0x02 },
	{ REG_VSTOP, 0x7a },	{ REG_VREF, 0x0a },
	{ REG_COM3, 0 },	{ REG_COM14, 0 },
	/* Mystery scaling numbers */
	{ 0x70, 0x3a },		{ 0x71, 0x35 },
	{ 0x72, 0x11 },		{ 0x73, 0xf0 },
	{ 0xa2, 0x02 },		{ REG_COM10, 0x0 },
	/* Gamma curve values */
	{ 0x7a, 0x20 },		{ 0x7b, 0x10 },
	{ 0x7c, 0x1e },		{ 0x7d, 0x35 },
	{ 0x7e, 0x5a },		{ 0x7f, 0x69 },
	{ 0x80, 0x76 },		{ 0x81, 0x80 },
	{ 0x82, 0x88 },		{ 0x83, 0x8f },
	{ 0x84, 0x96 },		{ 0x85, 0xa3 },
	{ 0x86, 0xaf },		{ 0x87, 0xc4 },
	{ 0x88, 0xd7 },		{ 0x89, 0xe8 },
	/* AGC and AEC parameters.  Note we start by disabling those features,
	   then turn them only after tweaking the values. */
	{ REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_BFILT },
	{ REG_GAIN, 0 },	{ REG_AECH, 0 },
	{ REG_COM4, 0x40 }, /* magic reserved bit */
	{ REG_COM9, 0x18 }, /* 4x gain + magic rsvd bit */
	{ REG_BD50MAX, 0x05 },	{ REG_BD60MAX, 0x07 },
	{ REG_AEW, 0x95 },	{ REG_AEB, 0x33 },
	{ REG_VPT, 0xe3 },	{ REG_HAECC1, 0x78 },
	{ REG_HAECC2, 0x68 },	{ 0xa1, 0x03 }, /* magic */
	{ REG_HAECC3, 0xd8 },	{ REG_HAECC4, 0xd8 },
	{ REG_HAECC5, 0xf0 },	{ REG_HAECC6, 0x90 },
	{ REG_HAECC7, 0x94 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC },
	/* Almost all of these are magic "reserved" values.  */
	{ REG_COM5, 0x61 },	{ REG_COM6, 0x4b },
	{ 0x16, 0x02 },		{ REG_MVFP, 0x07 },
	{ 0x21, 0x02 },		{ 0x22, 0x91 },
	{ 0x29, 0x07 },		{ 0x33, 0x0b },
	{ 0x35, 0x0b },		{ 0x37, 0x1d },
	{ 0x38, 0x71 },		{ 0x39, 0x2a },
	{ REG_COM12, 0x78 },	{ 0x4d, 0x40 },
	{ 0x4e, 0x20 },		{ REG_GFIX, 0 },
	{ 0x6b, 0x4a },		{ 0x74, 0x10 },
	{ 0x8d, 0x4f },		{ 0x8e, 0 },
	{ 0x8f, 0 },		{ 0x90, 0 },
	{ 0x91, 0 },		{ 0x96, 0 },
	{ 0x9a, 0 },		{ 0xb0, 0x84 },
	{ 0xb1, 0x0c },		{ 0xb2, 0x0e },
	{ 0xb3, 0x82 },		{ 0xb8, 0x0a },
	/* More reserved magic, some of which tweaks white balance */
	{ 0x43, 0x0a },		{ 0x44, 0xf0 },
	{ 0x45, 0x34 },		{ 0x46, 0x58 },
	{ 0x47, 0x28 },		{ 0x48, 0x3a },
	{ 0x59, 0x88 },		{ 0x5a, 0x88 },
	{ 0x5b, 0x44 },		{ 0x5c, 0x67 },
	{ 0x5d, 0x49 },		{ 0x5e, 0x0e },
	{ 0x6c, 0x0a },		{ 0x6d, 0x55 },
	{ 0x6e, 0x11 },		{ 0x6f, 0x9f }, /* "9e for advance AWB" */
	{ 0x6a, 0x40 },		{ REG_BLUE, 0x40 },
	{ REG_RED, 0x60 },
	{ REG_COM8, COM8_FASTAEC|COM8_AECSTEP|COM8_BFILT|COM8_AGC|COM8_AEC|COM8_AWB },
	/* Matrix coefficients */
	{ 0x4f, 0x80 },		{ 0x50, 0x80 },
	{ 0x51, 0 },		{ 0x52, 0x22 },
	{ 0x53, 0x5e },		{ 0x54, 0x80 },
	{ 0x58, 0x9e },
	{ REG_COM16, COM16_AWBGAIN },	{ REG_EDGE, 0 },
	{ 0x75, 0x05 },		{ 0x76, 0xe1 },
	{ 0x4c, 0 },		{ 0x77, 0x01 },
	{ REG_COM13, 0xc3 },	{ 0x4b, 0x09 },
	{ 0xc9, 0x60 },		{ REG_COM16, 0x38 },
	{ 0x56, 0x40 },
	{ 0x34, 0x11 },		{ REG_COM11, COM11_EXP|COM11_HZAUTO },
	{ 0xa4, 0x88 },		{ 0x96, 0 },
	{ 0x97, 0x30 },		{ 0x98, 0x20 },
	{ 0x99, 0x30 },		{ 0x9a, 0x84 },
	{ 0x9b, 0x29 },		{ 0x9c, 0x03 },
	{ 0x9d, 0x4c },		{ 0x9e, 0x3f },
	{ 0x78, 0x04 },
	/* Extra-weird stuff.  Some sort of multiplexor register */
	{ 0x79, 0x01 },		{ 0xc8, 0xf0 },
	{ 0x79, 0x0f },		{ 0xc8, 0x00 },
	{ 0x79, 0x10 },		{ 0xc8, 0x7e },
	{ 0x79, 0x0a },		{ 0xc8, 0x80 },
	{ 0x79, 0x0b },		{ 0xc8, 0x01 },
	{ 0x79, 0x0c },		{ 0xc8, 0x0f },
	{ 0x79, 0x0d },		{ 0xc8, 0x20 },
	{ 0x79, 0x09 },		{ 0xc8, 0x80 },
	{ 0x79, 0x02 },		{ 0xc8, 0xc0 },
	{ 0x79, 0x03 },		{ 0xc8, 0x40 },
	{ 0x79, 0x05 },		{ 0xc8, 0x30 },
	{ 0x79, 0x26 }
};

const uint8_t  ov7670_fmt_rgb565[12][2] = {
	{ REG_COM7, 0x0C },	/* Selects RGB mode */
	{ REG_RGB444, 0 },	/* No RGB444 please */
	{ REG_COM1, 0x0 },	/* CCIR601 */
	{ REG_COM15, COM15_RGB565 },
	{ REG_COM9, 0x38 }, 	/* 16x gain ceiling; 0x8 is reserved bit */
	{ 0x4f, 0xb3 }, 	/* "matrix coefficient 1" */
	{ 0x50, 0xb3 }, 	/* "matrix coefficient 2" */
	{ 0x51, 0    },		/* vb */
	{ 0x52, 0x3d }, 	/* "matrix coefficient 4" */
	{ 0x53, 0xa7 }, 	/* "matrix coefficient 5" */
	{ 0x54, 0xe4 }, 	/* "matrix coefficient 6" */
	{ REG_COM13, COM13_GAMMA|COM13_UVSAT }
	
};
const uint8_t ov7670_qcif_regs[15][2] = {
	{ REG_COM3, COM3_SCALEEN|COM3_DCWEN },
	{ REG_COM3, COM3_DCWEN },
	{ REG_COM14, COM14_DCWEN | 0x01},
	{ 0x73, 0xf1 },
	{ 0xa2, 0x52 },
	{ 0x7b, 0x1c },
	{ 0x7c, 0x28 },
	{ 0x7d, 0x3c },
	{ 0x7f, 0x69 },
	{ REG_COM9, 0x38 },
	{ 0xa1, 0x0b },
	{ 0x74, 0x19 },
	{ 0x9a, 0x80 },
	{ 0x43, 0x14 },
	{ REG_COM13, 0xc0 }
};

uint8_t ov7670_qqvga_regs[10][2] = {
	{ REG_COM3, COM3_DCWEN },
	{ REG_COM14, 0x1a},
    { 0x72, 0x22 },		// downsample by 4
    { 0x73, 0xf2 },		// divide by 4
	{ REG_HSTART, 0x16 },
    { REG_HSTOP, 0x04 },
    { REG_HREF, 0xa4 },
    { REG_VSTART, 0x02 },
    { REG_VSTOP, 0x7a },
    { REG_VREF, 0x0a },

};

void SCCB_Delay(void)
{
   unsigned int i=SCCB_Delay_Time;
   while(i)
   {
     i--;
   }
}
unsigned char I2C_ReceiveByte(void)  
{
    unsigned char i=8;
    unsigned char ReceiveByte=0;
    
    while(i--)
    {
      ReceiveByte<<=1;      
      SCLL;
      SCCB_Delay();
      SCLH;
      SCCB_Delay();
      if(SDAR)
      {
        ReceiveByte|=0x01;
      }
    }
    SCLL;
    return ReceiveByte;
}
void def_as_input(){
	  GPIO_InitTypeDef GPIO_InitStructure;
	
	 GPIO_InitStructure.GPIO_Pin = I2C_SDA; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
	
	
}
void def_as_output(){
	  GPIO_InitTypeDef GPIO_InitStructure;
	
	 GPIO_InitStructure.GPIO_Pin = I2C_SDA; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
	
}
	int sccb_write(uint8_t data){
	for(int i=0;i<8;i++){
	
		if(data<<i & 0x80){
			SDAH;
		}
		else{
			SDAL;
		}
		SCCB_Delay();
		SCLH;
		SCCB_Delay();
		SCLL;
		SCCB_Delay();
	}
	def_as_input();
    SCCB_Delay();
	SCLH;SCCB_Delay();  //Clock Uygulaniyor
	if(SDAR){
	 
		;SCLL;SCCB_Delay();
		def_as_output();
			return 0xFF;
	}
	;SCLL;SCCB_Delay();
	def_as_output();
	SCCB_Delay();
	return 0x00;
}
void sccb_repaid_start(){
		SDAL;
  SCCB_Delay();
  SCLH;
  SCCB_Delay();
  SDAH;
	SCCB_Delay();
	SDAL;
	SCCB_Delay();
  SCLL;
	SCCB_Delay();
	
}
void start(){
	SDAH;
	SCLH;
	SCCB_Delay();
	SDAL;
	SCCB_Delay();
  SCLL;
	SCCB_Delay();	
}
void stop(void)
{
SCLL;
 SDAL;
 SCCB_Delay();
 SCLH;
 SCCB_Delay();
 SDAH;
 SCCB_Delay();
}
int multi_write(uint8_t regadd,uint8_t data){
		start();
	SCCB_Delay();
	sccb_write(0x42);
	sccb_write((uint8_t)regadd);
	sccb_write(data);
		SCCB_Delay();
		stop();
		SCCB_Delay();
return 0xFF;

}
void init(){
		  GPIO_InitTypeDef GPIO_InitStructure;
	/* Enable GPIOB clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	/*SDA Pin Configuration*/
  GPIO_InitStructure.GPIO_Pin = I2C_SDA; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
	/*SCL Pin Configuration*/
	GPIO_InitStructure.GPIO_Pin = I2C_SCL; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(I2C_Port, &GPIO_InitStructure);
	//d12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
		//////////////////////////
	GPIO_InitTypeDef GPIO_InitStructure2;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure2);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_MCO);
  
  RCC_PLLI2SCmd(DISABLE);
  
  RCC_PLLI2SConfig(240, 5); // 192..432, 2..7, ie range of 27.429 Mhz to 192 MHz
  /* PLLI2SSCLK = 240 / 5 = 48Mhz */
	/* MCO2 Output Freq = 48 / 2 = 24Mhz*/
  RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2); // 24 MHz with default PLL fComp
  
  RCC_PLLI2SCmd(ENABLE);
  
  while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
	
	


}
int camera_init(){
	
	return multi_write(0x12,0x80);
}
void dcmi_init(){
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOE | 
                         RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOA, ENABLE); 

  RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure2;
	
  GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure2);
	
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure2);
	
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure2);
	
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_6;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure2);
	
	  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_DCMI);
		
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI);
	
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI);
		
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI);
		
	GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure2.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure2.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure2.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure2);
	
	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	GPIO_SetBits(GPIOD,GPIO_Pin_15);
	
	DCMI_InitTypeDef dcmi_init_type;
  dcmi_init_type.DCMI_CaptureMode=DCMI_CaptureMode_SnapShot;
  dcmi_init_type.DCMI_CaptureRate=DCMI_CaptureRate_All_Frame;
  dcmi_init_type.DCMI_ExtendedDataMode=DCMI_ExtendedDataMode_8b;
  dcmi_init_type.DCMI_HSPolarity=DCMI_HSPolarity_Low;//high
  dcmi_init_type.DCMI_VSPolarity=DCMI_VSPolarity_High;
  dcmi_init_type.DCMI_PCKPolarity=DCMI_PCKPolarity_Rising;
  dcmi_init_type.DCMI_SynchroMode=DCMI_SynchroMode_Hardware;
  DCMI_Init(&dcmi_init_type);
	
	DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
	DCMI_ITConfig(DCMI_IT_OVF, ENABLE);
	DCMI_ITConfig(DCMI_IT_ERR, ENABLE);

 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);  
 DMA_InitTypeDef  DMA_InitStructure;
  /* DMA2 Stream1 Configuration */  
  DMA_DeInit(DMA2_Stream1);

	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) (&DCMI->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) resim;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 176*144;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;		
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
      DMA_Init(DMA2_Stream1, &DMA_InitStructure);
      DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
      DMA_ITConfig(DMA2_Stream1, DMA_IT_TE, ENABLE);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}
 int ov_7670_init_for_snapShot(){;
	 for (int i=0;i<156;i++){

		if(!multi_write(OV7670_reg[i][0],OV7670_reg[i][1])){return 0x11;}
			  for(int z=0;z<100;z++)
    {
			SCCB_Delay();
		}
	 }
	 
	 	 for (int i=0;i<10;i++){

		if(!multi_write(ov7670_qqvga_regs[i][0],ov7670_qqvga_regs[i][1])){return 0x11;}
			  for(int z=0;z<100;z++)
    {
			SCCB_Delay();
		}
	 }
		 	 	 for (int i=0;i<12;i++){

		if(!multi_write( ov7670_fmt_rgb565[i][0], ov7670_fmt_rgb565[i][1])){return 0x11;}
			  for(int z=0;z<100;z++)
    {
			SCCB_Delay();
		}
	 }
	
	 

	 return 0xFF;
 }
 //////////////////////////
 
 void Serial_init(void) {

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // PA.2 USART2_TX, PA.3 USART2_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Set Alternate Function for RX and TX pins
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	USART_InitStructure.USART_BaudRate = 230400; //230400 works fine for me but 115200 might be a safer option
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// Enable interrupts for USART2 so that we can detect
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(USART2, ENABLE);
}

void Serial_log(char *s) {
	while (*s) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			; // Wait for Empty

		USART_SendData(USART2, *s++); // Send Char
	}
}




uint8_t Serial_read(void) {
	return USART_ReceiveData(USART2);
}

void Serial_sendb(volatile uint8_t val) {
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
		; // Wait for Empty
	USART_SendData(USART2, val);
}


//////////



void dumpFrame(void) {

	for (int i = 0; i < length; i++) {

			
			Serial_sendb(resim[i]);
		  for(int t=0;t<22;t++){
			SCCB_Delay();
			}
	
	}
	

			
	
}

