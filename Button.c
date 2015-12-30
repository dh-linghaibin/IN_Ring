
#include "Button.h"
#include "eeprom.h"
#include "Sys.h"
#include "Led.h"
#include "Com.h"
//up b
#define BUN_UP      PD_IDR_IDR2
#define BUN_DOWN    PD_IDR_IDR3
#define BUN_LEFT    PA_IDR_IDR2
#define BUN_RIGHT   PA_IDR_IDR1
#define BUN_MODE    PC_IDR_IDR3

static u8 mode = 0;

void ButtonInit(void) {
    PD_DDR_DDR2 = 0;
    PD_CR1_C12  = 1;
    PD_CR2_C22  = 0;
    
    PD_DDR_DDR3 = 0;
    PD_CR1_C13  = 1;
    PD_CR2_C23  = 0;
    
    PA_DDR_DDR1 = 0;
    PA_CR1_C11  = 1;
    PA_CR2_C21  = 0;
    
    PA_DDR_DDR2 = 0;
    PA_CR1_C12  = 1;
    PA_CR2_C22  = 0;
    
    PC_DDR_DDR3 = 0;
    PC_CR1_C13  = 1;
    PC_CR2_C23  = 1;
    
    EXTI_CR1 |= BIT(5);
    EXTI_CR1 &= ~BIT(4);
    
    if(EepromRead(0x20) == 0x55) {
        mode = EepromRead(0x21); 
    } else {
        EepromWrite(0x20,0x55);
        EepromWrite(0x21,0);
    }
}

void ButtonModeInt(u8 cmd) {
    PC_CR2_C23 = cmd;
}

u8 ButtonReadMode(void) {
    static u16 count = 0;
    if(BUN_MODE == 0) {
        if(count < 30000) {
            count++;
        } else {
            if(count == 30000){
                count = 0;
                return 0x44;  
            }
        }
    } else {
        if(count > 800) {
            count = 0;
            return 0x80;  
        }
        count = 0;
    }
    return 0x00;
}

u8 ButtonSetMode(void) {    
    if(mode < 2) {
        mode++;
    } else {
        mode = 0;
    }
    LedSetFlag(0);
    EepromWrite(0x21,mode);
    return mode;
}

u8 ButtonGetMode(void) {
    return mode;
}

static u8 button_flag = 0x00;

u8 ButtonGetFlag(void) {
    return button_flag;
}

u8 ButtonReadDirection(void)
{
    static u16 count1 = 0;
    static u16 count2 = 0;
    static u16 count3 = 0;
    static u16 count4 = 0;
    
    if(BUN_UP == 0) {
        if(count1 < 800) {
            count1++;
        } else {
            if(count1 == 800)
            {
                count1 = 900;
                button_flag = 0x01;
                return 0x01;  
            }
        }
    } else {
        count1 = 0;
    }
    
    if(BUN_DOWN == 0) {
        if(count2 < 800) {
            count2++;
        } else {
            if(count2 == 800)
            {
                count2 = 900;
                button_flag = 0x02;
                return 0x02;  
            }
        }
    } else {
        count2 = 0;
    }
    
    if(BUN_LEFT == 0) {
        if(count3 < 800) {
            count3++;
        } else {
            if(count3 == 800)
            {
                count3 = 900;
                button_flag = 0x04;
                return 0x04;  
            }
        }
    } else {
        count3 = 0;
    }
    
    if(BUN_RIGHT == 0) {
        if(count4 < 800) {
            count4++;
        } else {
            if(count4 == 800)
            {
                count4 = 900;
                button_flag = 0x08;
                return 0x08;  
            }
        }
    } else {
        count4 = 0;
    }
    return 0x00;
}

extern u8 sleep_flag;

#pragma vector=7
__interrupt void EXTI_PORTC_IRQHandler(void)
{
    if(sleep_flag == 1) {
        sleep_flag = 0;
        ComSendCmd(0x88,0x00,0x00,0);
    }
    return ;
}






