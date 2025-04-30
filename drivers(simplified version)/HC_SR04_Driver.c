#include "HC_SR04_Driver.h"

#include "iostm8s003f3.h"
#define TRIG    PC_ODR_ODR5  
#define ECHO    PC_IDR_IDR6
float temperature=32.0;


void GPIO_Init(void)
{
  PC_DDR_DDR5 = 1;      //PC2输出
  PC_CR1_C15 = 1;       //PC2推挽输出
  PC_CR1_C15 = 1;       //高速
  TRIG = 0;
  
  PC_DDR_DDR6 = 0;      // PC1输入模式
  PC_CR1_C16 = 0;       // PC1悬浮输入
  PC_CR2_C26 = 0;       // PC1关外部中断
}

void TIM4_Init(void)
{
    TIM4_PSCR = 0x07;   // 预分频器128，16MHz/128=125kHz(8us)
    TIM4_ARR = 255;     // 自动重装载值
    TIM4_CNTR = 0;      // 计数器清零
    TIM4_CR1 |= 0x01;   // 使能计数器
}

// HC-SR04初始化
void HC_SR04_Init(void)
{
    GPIO_Init();
    TIM4_Init();
    TRIG = 0;       // 初始Trig为低电平
}

// 获取距离(cm)
float HC_SR04_GetDistance(void)
{
    float time = 0;
    float distance = 0;
    
    // 发送10us的高电平触发信号
    TRIG = 1;
    Delay_us(15);       // 实际延时约15us
    TRIG = 0;
    
    // 等待回波信号变高
    while(ECHO == 0);
    
    // 开始计时
    TIM4_CNTR = 0;
    
    // 等待回波信号变低
    while(ECHO == 1);
    
    // 获取计时值
    time = TIM4_CNTR * 8;
    
    distance = time * (0.0331 + 0.00006*temperature) /2;
    
    return distance;
}