


#include "Led.h"

#define led_up      PC_ODR_ODR5
#define led_down    PC_ODR_ODR4
#define led_left    PB_ODR_ODR4
#define led_right   PB_ODR_ODR5
#define led_ring    PA_ODR_ODR3

void LedInit(void) {
    PB_DDR_DDR4 = 1;
    PB_CR1_C14  = 1;
    PB_CR2_C24  = 1;
    
    PB_DDR_DDR5 = 1;
    PB_CR1_C15  = 1;
    PB_CR2_C25  = 1;
    
    PC_DDR_DDR4 = 1;
    PC_CR1_C14  = 1;
    PC_CR2_C24  = 1;
    
    PC_DDR_DDR5 = 1;
    PC_CR1_C15  = 1;
    PC_CR2_C25  = 1;
    //ring Led
    PA_DDR_DDR3 = 1;
    PA_CR1_C13  = 1;
    PA_CR2_C23  = 1;
    
    TIM4_IER = 0x00;//       
	TIM4_EGR = 0x01;// 
	TIM4_PSCR = 0x07;// 计数器时钟=主时钟/128=2MHZ/128
	// 相当于计数器周期为64uS
	TIM4_ARR = 0x09;// 设定重装载时的寄存器值，255是最大值
	TIM4_CNTR = 0x00;// 设定计数器的初值
	// 定时周期=(ARR+1)*64=16320uS
	TIM4_IER = 0x01;//   
	TIM4_CR1 = 0x01;    
    
    LedSet(0x00);
}

static u8 led_up_duty = 0;
static u8 led_down_duty = 0;
static u8 led_left_duty = 0;
static u8 led_right_duty = 0;
static u8 led_ring_duty = 0;

void LedSet(u8 cmd) {
    switch(cmd) {
        case 0x01:
            led_up_duty = 50;
        break;
        case 0x02:
            led_down_duty = 50;
        break;
        case 0x04:
            led_left_duty = 50;
        break;
        case 0x08:
            led_right_duty = 50;
        break;
        default:break;
    }
}

void LedSetRing(u8 cmd) {
    static u16 count = 0;
    static u8 run_Falg = 0;
    static u8 Duty_Val = 0;
    count++;
    if(cmd == 0)
    {
        if(run_Falg == 0)
        {
            if(Duty_Val < 160)
            {
                if(count >= 1200)
                {
                    count = 0;
                    Duty_Val++;
                    led_ring_duty = Duty_Val;
                }
            }
            else
            {
                run_Falg = 1;
                count = 0;
            }
        }
        else if(run_Falg == 1)
        {
            if(count >= 2400)
            {
                run_Falg = 2;
            }
        }
        else if(run_Falg == 2)
        {
            if(Duty_Val > 0)
            {
                if(count >= 1200)
                {
                    count = 0;
                    Duty_Val--;
                    led_ring_duty = Duty_Val;
                }
            }
            else
            {
                run_Falg = 3;
                count = 0;
            }
        }
        else if(run_Falg == 3)
        {
            if(count >= 6000)
            {
                run_Falg = 0;
            }
        }
    }
    else if(cmd == 1)
    {   
        led_ring_duty = 60;
    }
    else if(cmd == 2)
    {
        led_ring_duty = 0;
    }
}

#pragma vector=0x19
__interrupt void TIM4_UPD_OVF_IRQHandler(void)
{
    static u8 cycle = 0;
    static u8 led_up_flag = 0;
    static u8 led_down_flag = 0;
    static u8 led_left_flag = 0;
    static u8 led_right_flag = 0;
    static u8 led_ring_flag = 0;
    
    TIM4_SR &= (~0x01);//清除中断标志
    
    if(cycle < 100) {
        cycle++;
    } else {
        cycle = 1;
        if(led_up_duty > 0) {
            static u8 count = 0;
            if(count < 10) {
                count++;
            } else {
                count = 0;
                led_up_duty -= 5;
            }
        }
        if(led_down_duty > 0) {
            static u8 count = 0;
            if(count < 10) {
                count++;
            } else {
                count = 0;
                led_down_duty -= 5;
            }
        }
        if(led_left_duty > 0) {
            static u8 count = 0;
            if(count < 10) {
                count++;
            } else {
                count = 0;
                led_left_duty -= 5;
            }
        }
        if(led_right_duty > 0) {
            static u8 count = 0;
            if(count < 10) {
                count++;
            } else {
                count = 0;
                led_right_duty -= 5;
            }
        }
    }
    //led up
    if(cycle > led_up_duty) {
		if(led_up_flag == 0) {
			led_up_flag = 1;
			led_up = 1;
		}
	} else {
		if(led_up_flag == 1) {
			led_up_flag = 0;
			led_up = 0;
		}
	}
    //led down
    if(cycle > led_down_duty) {
		if(led_down_flag == 0) {
			led_down_flag = 1;
			led_down = 1;
		}
	} else {
		if(led_down_flag == 1) {
			led_down_flag = 0;
			led_down = 0;
		}
	}
    //led left
    if(cycle > led_left_duty) {
		if(led_left_flag == 0) {
			led_left_flag = 1;
			led_left = 1;
		}
	} else {
		if(led_left_flag == 1) {
			led_left_flag = 0;
			led_left = 0;
		}
	}
    //led right
    if(cycle > led_right_duty) {
		if(led_right_flag == 0) {
			led_right_flag = 1;
			led_right = 1;
		}
	} else {
		if(led_right_flag == 1) {
			led_right_flag = 0;
			led_right = 0;
		}
	}
    //led ring
    if(cycle > led_ring_duty) {
		if(led_ring_flag == 0) {
			led_ring_flag = 1;
			led_ring = 0;
		}
	} else {
		if(led_ring_flag == 1) {
			led_ring_flag = 0;
			led_ring = 1;
		}
	}
    return;
}