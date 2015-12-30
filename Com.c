

#include "Com.h"
#include "delay.h"

#define COM_BIT_OUT 	PD_ODR_bit.ODR4
#define COM_BIT_IN 	 	PD_IDR_bit.IDR4
#define COM_BIT_DR 		PD_DDR_bit.DDR4
#define COM_BIT_INT 	PD_CR2_bit.C24

void ComInit(void)
{
    PD_DDR_DDR4 = 0;
    PD_CR1_C14  = 0;
    PD_CR2_C24  = 0;
    
    EXTI_CR1 |= BIT(7);
    EXTI_CR1 &= ~BIT(6);
}

u8 ComSend(u8 data[])
{
	u16 wait = 0;
	u8 data_t = 0;//保存临时值
	u8 i = 0,j = 0;
	
	COM_BIT_INT = 0;//中断
	COM_BIT_DR = 1;//设置为输出
	COM_BIT_OUT = 0;
	DelayUs(150);//拉低20ms说明总线开始
	COM_BIT_DR = 0;//设置为输入
	DelayUs(1);
	while(COM_BIT_IN == 1)//等待从机拉高
	{
		if(wait < 150)
		{
			wait++;
		}
		else//超时，退出
		{
			COM_BIT_INT = 1;//中断
			return 0;
		}
	}
	wait = 0;
	while(COM_BIT_IN == 0)
	{
		if(wait < 150)
		{
			wait++;
		}
		else//超时，退出
		{
			return 0;
		}
	}
	COM_BIT_DR = 1;//设置为输出
   // COM_BIT_OUT = 0;
  //  delayus(40);
  //  COM_BIT_OUT = 1;
  //  delayus(20);
	for(j = 0;j < 5;j++)
	{
		data_t = data[j];
		for(i=0;i<8;i++)
		{
			COM_BIT_OUT = 0;
			if(data_t&0x80)
			{
				DelayUs(120);
			}
			else 
			{
				DelayUs(40);
			}
			COM_BIT_OUT = 1;
			DelayUs(20);
			data_t<<=1;
		}
        //delayus(100);
	}
	DelayUs(60);//60
	
	COM_BIT_OUT = 1;
	
	COM_BIT_INT = 1;//中断
	COM_BIT_DR = 0;//设置为输入
	return 0;
}

volatile u16 wait_r = 0;//统计下拉时间

u8 ComRead(u8 data_s[])
{
	u8 i = 0,j = 0;
	u8 data = 0;
	wait_r = 0;
	
	COM_BIT_DR = 0;//设置为输入
	while(COM_BIT_IN == 0)
	{
		if(wait_r < 3000)wait_r++;
		else return 0x44;
	}
	if(wait_r > 120)
	{
		wait_r = 0;
		wait_r = 0;
		COM_BIT_DR = 1;//设置为输出
		COM_BIT_OUT = 0;
		DelayMs(25);
		COM_BIT_DR = 0;//设置为输入
		//开始接受数据
		for(j = 0;j < 5;j++)
		{
			for(i=0;i<8;i++)  
			{  
				data<<=1; 
				while(COM_BIT_IN == 1)
				{
					wait_r++;
					if(wait_r > 5500)//超时退出
					{
						return 0x44;
					}
				}
				wait_r = 0;
				while(COM_BIT_IN == 0)
				{
					if(wait_r < 40000)//超时退出
					{
						wait_r++;
					}
					else
					{
						return 0x44;
					}
				}
				if(wait_r > 20)//为1
				{
					data|=0x01;  
				}
				wait_r = 0;					
			}
			data_s[j] = data;
			data = 0;
		}
		if(data_s[4] == (data_s[0]+data_s[1]+data_s[2]+data_s[3]))//累加校验
		{
			if(data_s[4] != 0)
			{
				return 0x88;
			}
			else
			{
				return 0x44;
			}
		}
		else
		{
			return 0x44;
		}
	}
	else//时间不对 推出
	{
		return 0x44;
	}
}

void ComSendCmd(u8 cmd,u8 par1,u8 par2,u8 par3)
{
    u8 com_t_data[5] = {0,0,0,0,0};//前拨
	u16 com_check = 0;//保存累加校验值
	com_check = cmd+par1+par2+par3;
	com_t_data[4] = com_check;
	com_t_data[0] = cmd;
	com_t_data[1] = par1;
	com_t_data[2] = par2;
	com_t_data[3] = par3;
    INTOFF
	ComSend(com_t_data);
    INTEN
}


static u8 com_r_date[5];
static u8 rs_ok = 0;//whether it has received data

u8 ComGetArry(u8 num) {
    return com_r_date[num];
}

u8 ComGetFlag(void) {
    return rs_ok;
}

void ComClearFlag(void) {
    rs_ok = 0x00;
}


#pragma vector=8
__interrupt void EXTI_PORTD_IRQHandler(void)
{
    INTOFF;//关总中断
    
	if(ComRead(com_r_date) == 0x88)
	{
		rs_ok = 0x88;
	}
    
	INTEN;//开总中断
    return ;
}