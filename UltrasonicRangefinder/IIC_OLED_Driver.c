#include "IIC_OLED_Driver.h"
#include "oledfont.h"

#include "iostm8s105c6.h"
#define SCL PB_ODR_ODR5
#define SDA PB_ODR_ODR4

//软件模拟IICs
void IIC_Init(void)
{
    //配置SCL和SDA引脚为推挽高速输出模式
    PB_DDR_DDR4 = 1;     // 设置PC1为输出
    PB_CR1_C14 = 1;     // 推挽输出
    PB_CR2_C24 = 1;     // 高速输出
    
    PB_DDR_DDR5 = 1;     // 设置PC2为输出
    PB_CR1_C15 = 1;     // 推挽输出
    PB_CR2_C25 = 1;     // 高速输出
    
    SCL=1;
    SDA=1;
    
    Delay_us(5);
}

void IIC_Start()
{
  //在SCL为高电平时SDA出现下降沿
  SDA=1;
  SCL=1;
  Delay_us(5);
  SDA=0;
  Delay_us(5);
  SCL=0;
}

void IIC_Stop()
{
  //在SCL为高电平时SDA出现上升沿
  SDA=0;
  SCL=1;
  Delay_us(5);
  SDA=1;
  Delay_us(5);
}

void IIC_Wait_Ack()
{
  SCL=1;
  SCL=0;
}

void Write_IIC_Byte(unsigned char IIC_Byte)
{
	u8 i;
	u8 mask,data;
        //IIC发送的第一个字节数据为7个地址与一个读写操作位
	data=IIC_Byte;
	SCL=0;
	for(i=0;i<8;i++)		
	{
            mask=data;
            mask=mask&0x80;
            if(mask==0x80)
            {SDA=1;}
            else 
            {SDA=0;}
            data=data<<1;
            SCL=1;
            SCL=0;
        }
}

void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);       //Slave address,SA0=0
    IIC_Wait_Ack();	
    Write_IIC_Byte(0x00);	//write command
    IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Command); 
    IIC_Wait_Ack();	
    IIC_Stop();
}

void Write_IIC_Data(unsigned char IIC_Data)
{
    //asm("sim");
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();	
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();	
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();	
    IIC_Stop();
    //asm("rim");
}

//OLED驱动函数
void OLED_WR_Byte(u8 dat,u8 cmd)
{
  if(cmd)
  {
    Write_IIC_Data(dat);
  }
  else
  {
    Write_IIC_Command(dat);
  }
}

void OLED_Set_Pos(unsigned char x, unsigned char y) 
{ 
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD); 
}   	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//清屏
void OLED_Clear(void)  
{  
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	} //更新显示
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr)
{      	
	unsigned char c=0,i=0;	
        c=chr-' ';//得到偏移后的值			
        if(x>Max_Column-1){x=0;y=y+2;}
        if(SIZE ==16)
        {
          OLED_Set_Pos(x,y);	
          for(i=0;i<8;i++)
          OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
          OLED_Set_Pos(x,y+1);
          for(i=0;i<8;i++)
          OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
        }
        else {	
          OLED_Set_Pos(x,y+1);
          for(i=0;i<6;i++)
          OLED_WR_Byte(F6x8[c][i],OLED_DATA);   
        }
}
//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ');
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0'); 
	}
} 
//显示字符串
void OLED_ShowString(u8 x,u8 y,u8 *chr)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		
          OLED_ShowChar(x,y,chr[j]);
          x+=8;
          if(x>120){x=0;y+=2;}
          j++;
	}
}

void OLED_Init(void)
{
  IIC_Init();

  OLED_WR_Byte(0xAE,OLED_CMD);//--turn  oled panel
  OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
  OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
  OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
  OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
  OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
  OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00,OLED_CMD);//-not offset
  OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
  OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
  OLED_WR_Byte(0x12,OLED_CMD);
  OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
  OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
  OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02,OLED_CMD);//
  OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
  OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
  OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
  OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
  
  OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
  OLED_Clear();
  OLED_Set_Pos(0,0); 	
}  
