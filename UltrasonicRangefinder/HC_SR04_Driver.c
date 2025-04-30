#include "HC_SR04_Driver.h"

#include "iostm8s105c6.h"
#define TRIG    PC_ODR_ODR2  // PC2作为Trig输出
#define ECHO    PC_IDR_IDR1
float temperature=32.0;

void HC_SR04_Init(void)
{
  //配置TRIG引脚为推挽输出
  PC_DDR_DDR2 = 1;      //PC2输出
  PC_CR1_C12 = 1;       //PC2推挽输出
  PC_CR1_C12 = 1;       //高速
  TRIG = 0;
  //配置ECHO引脚为上拉输入
  //ECHO为PC1(即TIM1_CH1通道)
  PC_DDR_DDR1 = 0;      // PC1输入模式
  PC_CR1_C11 = 0;       // PC1悬浮输入
  PC_CR2_C21 = 0;       // PC1关外部中断
  //配置TIM1为输入捕获模式
  TIM1_Init();
  
  Delay_us(15);
}

//使用TIM1输入捕获的复位触发模式对echo引脚返回的高电平持续时间进行测量
void TIM1_Init(void)
{
    TIM1_PSCRH = 0x00;
    TIM1_PSCRL = 0x02;
    TIM1_ARRH = 0xFF;
    TIM1_ARRL = 0xFF;   
    
    //利用输入时TIx产生的两路相同信号TIxFP1和TIxFP2，对上边沿与下边沿进行检测
    TIM1_CCMR1 = 0x01;  //CC1S=01 TI1FP1连接到IC1
    TIM1_CCMR2 = 0x02;  //CC2S=10 TI1FP2连接到IC2
    TIM1_CCER1_CC1P = 0;        //检测TI1FP1上升沿
    TIM1_CCER1_CC2P = 1;        //检测TI1FP2下降沿
    TIM1_SMCR = 0x54;   //TS=101(TI1FP1为触发信号), SMS=100(复位触发模式)
    //从TI1的上升沿触发复位，在捕获到TI2的下降沿时
    TIM1_CCER1 |= 0x11;    //使能两个捕获
 
    TIM1_CR1 = 0x01;    // 使能计数器
}

float time=0;
float HC_SR04_GetDistance(void)
{
    TIM1_SR1 &= 0xF9; //清除CC1IF和CC2IF标志位
    TIM1_CCER1 |= 0x11; //开启捕获功能
    
    float distance = 0;
    u16 pulse_width = 0;
    
    // 发送高电平触发信号
    TRIG = 1;
    Delay_us(15);
    TRIG = 0;

    while((TIM1_SR1&0x02)==0); //等待捕获比较标志位CC1IF变为1(上升沿)
    while((TIM1_SR1&0x04)==0); //等待捕获比较标志位CC2IF变为1(下降沿)
    
    //IC1检测到上升沿时复位计时，到IC2检测到下降沿时即高电平脉宽
    pulse_width = (u16)TIM1_CCR2H<<8; //取高电平脉宽的高8位
    pulse_width |= TIM1_CCR2L; //取高电平脉宽的低8位
    
    time=(float)(pulse_width)*0.1875;//主频=16M，TIM1进行三分频
    //声速=340 00 cm/s=0.034cm/us -> dis = (time * 0.034cm/us) / 2
    //distance = time*0.034/2;
    
    //考虑温度系数：c=331+0.6T -> dis = time * (0.0331 + 0.00006T) /2
    distance = time * (0.0331 + 0.00006*temperature) /2;
    
    return distance;
}