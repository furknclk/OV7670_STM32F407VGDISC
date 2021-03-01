
	 #include "stm32f4xx.h"
	 #include <string.h>
	 #include <stdio.h>
	 #include "ov7670.h"

		int inn=0;
		int camera_snap_ready=0;


static volatile int frame_flag = 0;
static volatile int send_sync_frame = 0;	 
void DCMI_IRQHandler(void) {
	if (DCMI_GetFlagStatus(DCMI_FLAG_FRAMERI) == SET) { 
		DCMI_ClearFlag(DCMI_FLAG_FRAMERI);
		DMA_Cmd(DMA2_Stream1, DISABLE);
		DCMI_Cmd(DISABLE);
		DCMI_CaptureCmd(DISABLE);
				   
	}
	if (DCMI_GetFlagStatus(DCMI_FLAG_OVFRI) == SET) {
		DCMI_ClearFlag(DCMI_FLAG_OVFRI);
	}
	if (DCMI_GetFlagStatus(DCMI_FLAG_ERRRI) == SET) {
		DCMI_ClearFlag(DCMI_FLAG_ERRRI);
	}
}

void DMA2_Stream1_IRQHandler(void) {
	
	// DMA complete
	if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) != RESET) { // Transfer complete
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
		 frame_flag = 1;
	
	} else if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TEIF1) != RESET) { // Transfer error
		// Not used, just for debug
		DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TEIF1);
	}

	
}







/////
int a_12;

	int main(){
	init();
	Serial_init();
	dcmi_init();
	inn=camera_init();
	for(int z=0;z<10000;z++){	//30000
	SCCB_Delay();
  }

		
	camera_snap_ready=ov_7670_init_for_snapShot();
   while(!camera_snap_ready);
		DMA_Cmd(DMA2_Stream1, ENABLE);
	 start();
	  sccb_write(0x42);
		sccb_write(REG_COM7);
	  stop();
	  start();
	  sccb_write(0x43);
	  def_as_input();
	  a_12=I2C_ReceiveByte();
		SCCB_Delay();
		DCMI_Cmd(ENABLE);
		SCCB_Delay();
		DCMI_CaptureCmd(ENABLE);
	
   

		while (1) {
		
			
		if (frame_flag == 1) {
		
			for(int z=0;z<10000;z++){	//20
	SCCB_Delay();
  }
     dumpFrame();
				frame_flag = 0;
			SCCB_Delay();
			SCCB_Delay();
			
		}

	}
		
}
	
