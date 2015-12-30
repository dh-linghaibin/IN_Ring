
#include "Sys.h"
#include "Com.h"
#include "Button.h"
#include "delay.h"
#include "Led.h"

void SysInit(void) {
    CLK_CKDIVR=0x00;//ʱ��Ԥ��Ƶ��Ĭ��8���䣬0x18.16M-0x00��8M-0x08;4M-0x10;
}

u8 sleep_flag = 0;

void SysSleep(void) {
    
    LedSetRing(3);
    ComSendCmd(0x44,0x00,0,0);
    //�������ܽ��͹���
    CLK_ICKR |= 0x20;
    FLASH_CR1 &= ~0x04;

    while(PC_IDR_IDR3 == 0);
    DelayMs(1000);
    ButtonModeInt(1);
    sleep_flag = 1;
    MCUSLEEP
}