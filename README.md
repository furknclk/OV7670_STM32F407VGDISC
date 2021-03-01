# OV7670_STM32F407VGDISC
Ov7670 camera module example on stm32f407vg discovery board

Ov7670 capture an image for once. And than stm32 dumps the frame on USART. In my problem i have to make it slow. But if you want a speeder communication, basicly just delete the delay func which is on the dump_frame function.

CAMERA CONFIG.
QQVGA,RGB565

PIN DESCRIPTION
D0 PC6
D1 PC7
D2 PC8
D3 PE1
D4 PC11
D5 PB6
D6 PE5
D7 PE6
VS PB7
HS PA4
PCLK PA6
XCLK PC9
SCL PB8
SDA PB9

RX A3
TX A2
