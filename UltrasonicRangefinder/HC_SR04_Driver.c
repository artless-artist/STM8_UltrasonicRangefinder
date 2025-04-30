#include "HC_SR04_Driver.h"

#include "iostm8s105c6.h"
#define TRIG    PC_ODR_ODR2  // PC2��ΪTrig���
#define ECHO    PC_IDR_IDR1
float temperature=32.0;

void HC_SR04_Init(void)
{
  //����TRIG����Ϊ�������
  PC_DDR_DDR2 = 1;      //PC2���
  PC_CR1_C12 = 1;       //PC2�������
  PC_CR1_C12 = 1;       //����
  TRIG = 0;
  //����ECHO����Ϊ��������
  //ECHOΪPC1(��TIM1_CH1ͨ��)
  PC_DDR_DDR1 = 0;      // PC1����ģʽ
  PC_CR1_C11 = 0;       // PC1��������
  PC_CR2_C21 = 0;       // PC1���ⲿ�ж�
  //����TIM1Ϊ���벶��ģʽ
  TIM1_Init();
  
  Delay_us(15);
}

//ʹ��TIM1���벶��ĸ�λ����ģʽ��echo���ŷ��صĸߵ�ƽ����ʱ����в���
void TIM1_Init(void)
{
    TIM1_PSCRH = 0x00;
    TIM1_PSCRL = 0x02;
    TIM1_ARRH = 0xFF;
    TIM1_ARRL = 0xFF;   
    
    //��������ʱTIx��������·��ͬ�ź�TIxFP1��TIxFP2�����ϱ������±��ؽ��м��
    TIM1_CCMR1 = 0x01;  //CC1S=01 TI1FP1���ӵ�IC1
    TIM1_CCMR2 = 0x02;  //CC2S=10 TI1FP2���ӵ�IC2
    TIM1_CCER1_CC1P = 0;        //���TI1FP1������
    TIM1_CCER1_CC2P = 1;        //���TI1FP2�½���
    TIM1_SMCR = 0x54;   //TS=101(TI1FP1Ϊ�����ź�), SMS=100(��λ����ģʽ)
    //��TI1�������ش�����λ���ڲ���TI2���½���ʱ
    TIM1_CCER1 |= 0x11;    //ʹ����������
 
    TIM1_CR1 = 0x01;    // ʹ�ܼ�����
}

float time=0;
float HC_SR04_GetDistance(void)
{
    TIM1_SR1 &= 0xF9; //���CC1IF��CC2IF��־λ
    TIM1_CCER1 |= 0x11; //����������
    
    float distance = 0;
    u16 pulse_width = 0;
    
    // ���͸ߵ�ƽ�����ź�
    TRIG = 1;
    Delay_us(15);
    TRIG = 0;

    while((TIM1_SR1&0x02)==0); //�ȴ�����Ƚϱ�־λCC1IF��Ϊ1(������)
    while((TIM1_SR1&0x04)==0); //�ȴ�����Ƚϱ�־λCC2IF��Ϊ1(�½���)
    
    //IC1��⵽������ʱ��λ��ʱ����IC2��⵽�½���ʱ���ߵ�ƽ����
    pulse_width = (u16)TIM1_CCR2H<<8; //ȡ�ߵ�ƽ����ĸ�8λ
    pulse_width |= TIM1_CCR2L; //ȡ�ߵ�ƽ����ĵ�8λ
    
    time=(float)(pulse_width)*0.1875;//��Ƶ=16M��TIM1��������Ƶ
    //����=340 00 cm/s=0.034cm/us -> dis = (time * 0.034cm/us) / 2
    //distance = time*0.034/2;
    
    //�����¶�ϵ����c=331+0.6T -> dis = time * (0.0331 + 0.00006T) /2
    distance = time * (0.0331 + 0.00006*temperature) /2;
    
    return distance;
}