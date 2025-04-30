# 简介

* 基于STM8105C6芯片控制，使用HC-SR04超声波模块进行测距，带有OLED显示屏(由SSD1306显示芯片驱动)用以显示测距结果的超声波测距仪
* 代码为C语言编写的STM8 HC-SR04驱动与SSD1306显示驱动

### 功能

* 实时测距，将结果以cm与m为单位，保留两位小数输出到OLED上
* 温度参数修正：改变`HC_SR04_Driver.c`中的`temperature`变量(单位为摄氏度)即可修正测距结果
* 测距启停：按下按键，将会停止测距，OLED保持显示按下按键时的测距结果

# 引脚连接

需要使用单片机的6个引脚，修改各个.c中文件中宏定义与对应的GPIO初始化代码，即可改变引脚连接

### IIC_OLED_Driver.c

* IIC时钟线：SCL -> PB5
* IIC数据线：SDA -> PB4

### HC_SR04_Driver.c:

* 测距触发引脚：TRIG -> PC2
* 测距结果返回引脚：ECHO -> PC1（ECHO引脚必须连接到TIM1_CH1复用引脚上）

  也提供了不使用输入捕获功能，而是使用基本计数器的`HC_SR04_Driver_TIM4.c`

### main.c:

* 开关按键：key -> PA3
* 工作状态灯：led -> PB0

# 代码移植

1. 根据实际的引脚连接情况修改对引脚的宏定义与初始化
2. 将各个.c文件中的头文件`iostm8s105c6.h`修改为实际的单片机型号
3. (使用IAR)在项目Options - General Options - Target中将Device选择为实际的单片机型号

### 入门级单片机移植须知

* 使用内存较小的单片机时，无法储存完整的OLED字库文件，请使用提供的简化版`oledfont.h`(只能显示数字)，并在`IIC_OLED_Driver.h`中将宏定义`SIZE`数值改为8
* (使用IAR)在项目Options - C/C++ Compiler - level中将优化等级调为High
* 如果出现卡死情况，请配置看门狗
