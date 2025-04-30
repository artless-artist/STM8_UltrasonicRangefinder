#include "HC_SR04_Driver.h"

#include "iostm8s003f3.h"
#define TRIG    PC_ODR_ODR5  
#define ECHO    PC_IDR_IDR6
float temperature=32.0;


void GPIO_Init(void)
{
  PC_DDR_DDR5 = 1;      //PC2���
  PC_CR1_C15 = 1;       //PC2�������
  PC_CR1_C15 = 1;       //����
  TRIG = 0;
  
  PC_DDR_DDR6 = 0;      // PC1����ģʽ
  PC_CR1_C16 = 0;       // PC1��������
  PC_CR2_C26 = 0;       // PC1���ⲿ�ж�
}

void TIM4_Init(void)
{
    TIM4_PSCR = 0x07;   // Ԥ��Ƶ��128��16MHz/128=125kHz(8us)
    TIM4_ARR = 255;     // �Զ���װ��ֵ
    TIM4_CNTR = 0;      // ����������
    TIM4_CR1 |= 0x01;   // ʹ�ܼ�����
}

// HC-SR04��ʼ��
void HC_SR04_Init(void)
{
    GPIO_Init();
    TIM4_Init();
    TRIG = 0;       // ��ʼTrigΪ�͵�ƽ
}

// ��ȡ����(cm)
float HC_SR04_GetDistance(void)
{
    float time = 0;
    float distance = 0;
    
    // ����10us�ĸߵ�ƽ�����ź�
    TRIG = 1;
    Delay_us(15);       // ʵ����ʱԼ15us
    TRIG = 0;
    
    // �ȴ��ز��źű��
    while(ECHO == 0);
    
    // ��ʼ��ʱ
    TIM4_CNTR = 0;
    
    // �ȴ��ز��źű��
    while(ECHO == 1);
    
    // ��ȡ��ʱֵ
    time = TIM4_CNTR * 8;
    
    distance = time * (0.0331 + 0.00006*temperature) /2;
    
    return distance;
}