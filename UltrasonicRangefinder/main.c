#include "iostm8s105c6.h"
#define key PA_IDR_IDR3
#define led PB_ODR_ODR0

#include "stdio.h"
#include "IIC_OLED_Driver.h"
#include "HC_SR04_Driver.h"

int main( void )
{
  PA_DDR_DDR3 = 0;//���ð���
  PA_CR1_C13 = 1;
  PA_CR2_C23 = 0;
  PB_DDR_DDR0 = 1;
  PB_CR1_C11 = 1;
  u8 pause = 1;
  
  char buf[20];//�ַ���ʾ������
  float dis_temp=0,dis_sum=0;//����˲�
  u8 cnt=0,cnt_err=0;
  
  float dis_cm=1.2345;//������Ϊ��λ�Ĳ������
  float dis_m=1.2345;//����Ϊ��λ
  
  CLK_SWR = 0xE1;       //����ΪHSI 16MHZ
  CLK_CKDIVR = 0x00;    //����Ƶ
  HC_SR04_Init();
  OLED_Init();
  OLED_Clear();

  while(1)
  {
    dis_sum=0;
    while(cnt<10)
    {
      dis_temp=0;
      dis_temp=HC_SR04_GetDistance();
      if(dis_temp<400)//����������
      {
        dis_sum+=dis_temp;
      }else{
        cnt_err++;
      }
      cnt++;
      Delay_ms(20);
    }
    cnt=0;
    if(cnt_err<6)
    {
      if(pause==1)
      {
        dis_cm=dis_sum/10;
        dis_m=dis_cm/100;
        
        Delay_ms(200);
      }
    }else{
      dis_cm=0;
      dis_m=0;
      
      Delay_ms(200);
    }
    cnt_err=0;
    sprintf(buf,"lenth:%.2fcm  ",dis_cm);
    OLED_ShowString(0,1,(uint8_t *)buf);

    
    sprintf(buf,"lenth:%.2fm  ",dis_m);
    OLED_ShowString(0,4,(uint8_t *)buf);
    
    
    if(key==0)
    {
            Delay_ms(10);		//�����ʱ���10ms����
            if(key==0)
            {
                    led = ~led;
                    pause++;
                    if(pause>1)
                    {pause=0;}
                    while(key==0);
            }
    }       
  }
}