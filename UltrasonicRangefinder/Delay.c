#include "Delay.h"

void Delay_us(unsigned int time_us)
{
    while(time_us--)
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
}

void Delay_ms(unsigned int time_ms)
{
    unsigned int i, j;
    for (i = 0; i < time_ms; i++)
    {
        for (j = 0; j < 1600; j++);
    }
}