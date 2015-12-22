


#include "Encouder.h"
#include "delay.h"

#define IN_A PC_IDR_IDR6
#define IN_B PC_IDR_IDR7

void EncouderInit(void) {
    PC_DDR_DDR6 = 0;
    PC_CR1_C16  = 1;
    PC_CR2_C26  = 0;
    
    PC_DDR_DDR7 = 0;
    PC_CR1_C17  = 1;
    PC_CR2_C27  = 0;
    
    EXTI_CR1 |= BIT(5);
	EXTI_CR1 &= ~BIT(4);

}


static const s8 encoder_tab[4][4]={
{0,1,-1,0},
{-1,0,0,1},                
{1,0,0,-1},
{0,-1,1,0},        
};

static s8 EncoderRead(void)
{
    static u8 encoder_now;//本次值
    static u8 encoder_ago;//上次值
    s8 a=0;
    u8 encoder_filter=0;  //滤波
    while(a<10)//判断次数,进行滤波
    {
			encoder_now=IN_A;
			encoder_now<<=1;
			encoder_now+=IN_B;
			if(encoder_filter==encoder_now)
			{
				a++;
			}
			else 
			{
				a=0;
			}
			encoder_filter=encoder_now;
    }
		
    a=encoder_tab[encoder_now][encoder_ago];
    encoder_ago=encoder_now;
    return a;
}

u8 EncouderFlag(void) {
    s8 data = 0;
    data = EncoderRead();
    if(data == 1) {
		DelayMs(20);
        EncoderRead();
        return 0x80;
    } else if(data == -1) {
        DelayMs(20);
        EncoderRead();
        return 0x40;
    }
    return 0x00;
}

