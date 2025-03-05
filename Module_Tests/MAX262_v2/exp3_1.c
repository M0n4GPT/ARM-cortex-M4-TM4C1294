//*****************************************************************************

//*****************************************************************************

//*****************************************************************************
//
// 头文件
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义和函数原型
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型
#include "driverlib/uart.h"       // 与UART有关的宏定义和函数原型
#include "uartstdio.h"       // 与UART有关的宏定义和函数原型

#include "tm1638.h"               // 与控制TM1638芯片有关的宏定义和函数原型
#include "driverlib/adc.h"
#include "max262.h"               // 与控制max262芯片有关的宏定义和函数原型
//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T100ms	5                  // 0.1s软件定时器溢出值，5个20ms

#define GPIO_PA0_U0RX           0x00000001	//补充两个宏定义，不知道为什么pin_map.h里的没有被识别到
#define GPIO_PA1_U0TX           0x00000401
#define usec 5
//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断
void UARTInit(void);        // UART初始化
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// 向UART发送字符串
void UART0_Handler(void);
void ADC0Init(void);
uint32_t ADC_Sample(void);
uint32_t ADC_sinSample(uint8_t keyvalue);
void SentFreq(void);
void Set_Freq(uint32_t Freqency);
void delay_sys(uint32_t delayunit);
void delay_usec(uint32_t _usec);
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************

// 软件定时器计数
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;


// 软件定时器溢出标志
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;


// 系统时钟频率
uint32_t ui32SysClock;
//ADC
uint32_t ADC0value1 = 0;
char ADCstring[6] = "0000\r\n";
uint32_t timeflag = 0;

//AD9850
uint32_t freq = 0;
uint8_t Control_AD9850 = 0x00;
uint8_t debugflag = 0;

//mcu按键
uint8_t ui8KeyValue = 1;
uint8_t ui8KeyState = 1;
// tm1638按键值
uint8_t key_code = 0;

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为 0、1、2、3、4、5、6、7
uint8_t digit[8]={'_',' ','_',' ',' ',' ',' ',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为0、1、2、3、4、5、6、7
uint8_t pnt = 0x00;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为0、1、2、3、4、5、6、7
//     对应元件LED1、LED2、LED3、LED4、LED5、LED6、LED7、LED8
uint8_t led[] = {1, 0, 0, 0, 0, 0, 0, 0};

//*****************************************************************************
//
//宏定义
//
//*****************************************************************************
#define AD9850_D7_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define AD9850_D7_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0)
#define AD9850_FQ_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, GPIO_PIN_1)
#define AD9850_FQ_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0)
#define AD9850_WCLK_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define AD9850_WCLK_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, 0)
#define AD9850_RESET_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define AD9850_RESET_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_3, 0)

//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{
    uint8_t uart_receive_char;
    DevicesInit();            //  MCU器件初始化
		
		while (clock100ms < 3);   // 延时>60ms,等待TM1638上电完成
		TM1638_Init();	          // 初始化TM1638
	
    ADC0Init();

    AD9850_RESET_H;
    AD9850_RESET_H;
    AD9850_RESET_L;
    Set_Freq(1000);
    delay_sys(10);
    Set_Freq(1000);
    delay_sys(10);
		MAX262_LH(10000.000,0.707,highPass,modeThree,channelA); 
	  delay_sys(10);
    while (1)
    {
        ui8KeyValue = GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0);
        ADC0value1 = ADC_sinSample(ui8KeyValue);
    }

}

//*****************************************************************************
//
// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。
//						使能PortL。
//          （PL0-D7,PL1-FQUD,PL2-WCLK,PL3-RESET）
//						使能PortK，设置PK4,PK5为输出；使能PortM，设置PM0为输出。
//          （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK）
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void GPIOInit(void)
{
		//////配置DDS模块管脚
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);				// 使能端口 L
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {};		// 等待端口 L准备完毕

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {};

    // 设置端口 L的第0,1,2,3位为AD9850的输出引脚,PL0-D7,PL1-FQUD,PL2-WCLK,PL3-RESET
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    // 设置PJ0
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0);
    // PJ0设置为推挽上拉
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

			
		//////配置TM1638芯片管脚
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// 使能端口 K	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// 等待端口 K准备完毕		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// 使能端口 M	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// 等待端口 M准备完毕		
		
		// 设置端口 K的第4,5位（PK4,PK5）为输出引脚		PK4-STB  PK5-DIO
		GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
		// 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);	
		
		
		//////配置MAX262芯片管脚
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);						// 使能端口 H	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH)){};		// 等待端口 H准备完毕		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// 使能端口 M	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// 等待端口 M准备完毕		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);						// 使能端口 P
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP)){};		// 等待端口 P准备完毕		
		
		// 设置端口 H的第0,1位（PH0,PH1）为输出引脚		PH0-A2  PH1-A3
		GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
		// 设置端口 M的第1,2,6,7位（PM1,2,6,7）为输出引脚   PM1-A0 PM2-A1 PM6-OE PM7-D0
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_6|GPIO_PIN_7);	
		// 设置端口 P的5位（PP5）为输出引脚   PP5-D1
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_5);	
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);						// 使能端口 N
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){};		// 等待端口N准备完毕		
		
		// 设置端口 H的第0,1位（PN3）为输出引脚		PN3-CLK
		GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3);
}

//*****************************************************************************
//
// 函数原型：SysTickInit(void)
// 函数功能：设置SysTick中断
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // 设置心跳节拍,定时周期20ms
    SysTickEnable();  			// SysTick使能
    SysTickIntEnable();			// SysTick中断允许
}

//*****************************************************************************
//
// 函数原型：void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// 函数功能：向UART模块发送字符串
// 函数参数：ui32Base：UART模块
//          cMessage：待发送字符串
// 函数返回值：无
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0')
        UARTCharPut(ui32Base, *(cMessage++));
}

//*****************************************************************************
//
// 函数原型：void UARTInit(void)
// 函数功能：UART初始化。使能UART0，设置PA0,PA1为UART0 RX,TX引脚；
//          设置波特率及帧格式。
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void UARTInit(void)
{
    // 引脚配置
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // 使能UART0模块
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // 使能端口 A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// 等待端口 A准备完毕

    GPIOPinConfigure(GPIO_PA0_U0RX);				  // 设置PA0为UART0 RX引脚
    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // 设置PA1为UART0 TX引脚

    // 设置端口 A的第0,1位（PA0,PA1）为UART引脚
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // 波特率及帧格式设置
    UARTConfigSetExpClk(UART0_BASE,
                        ui32SysClock,
                        115200,                  // 波特率：115200
                        (UART_CONFIG_WLEN_8 |    // 数据位：8
                         UART_CONFIG_STOP_ONE |  // 停止位：1
                         UART_CONFIG_PAR_NONE)); // 校验位：无

    // 初始化完成后向PC端发送"Hello, 2A!"字符串
    UARTStringPut(UART0_BASE, (const char *)"\r\nHello\r\n");
    IntEnable(21); // UART0
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);//
}

void UART0_Handler(void)
{
//int32_t availflag = 0;
//int32_t uart0_int_status;
//uint8_t uart_receive_char;
// uart0_int_status = UARTIntStatus(UART0_BASE, true); // ?????
// UARTIntClear(UART0_BASE, uart0_int_status); // ?????
//	while(UARTCharsAvail(UART0_BASE))
//	{
//		availflag = 1;
//		uart_receive_char = UARTCharGetNonBlocking(UART0_BASE);
//	}
// if(availflag) // ????? FIFO ????
// {

//}

}


//*****************************************************************************
//
// 函数原型：DevicesInit(void)
// 函数功能：CU器件初始化，包括系统时钟设置、GPIO初始化和SysTick中断设置
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void DevicesInit(void)
{
    // 使用外部25MHz主时钟源，经过PLL，然后分频为20MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),
                                      20000000);

    GPIOInit();             // GPIO初始化
    SysTickInit();          // 设置SysTick中断
    UARTInit();             // UART初始化
    IntMasterEnable();			// 总中断允许
}

//*****************************************************************************
//
// 函数原型：void SysTick_Handler(void)
// 函数功能：SysTick中断服务程序
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void SysTick_Handler(void)       // 定时周期为20ms
{
//	timeflag++;
//				if(timeflag > 25)
// {
//		ADC0value1 = ADC_Sample();
//		timeflag = 0;
//		ADCstring[0] = ADC0value1 / 1000 + '0';
//		ADCstring[1] = ADC0value1 % 1000 / 100 + '0';
//		ADCstring[2] = ADC0value1 % 100 / 10 + '0';
//		ADCstring[3] = ADC0value1 % 10 + '0';
//		UARTCharPutNonBlocking(UART0_BASE, ADCstring[0]);
//		UARTCharPutNonBlocking(UART0_BASE, ADCstring[1]);
//		UARTCharPutNonBlocking(UART0_BASE, ADCstring[2]);
//		UARTCharPutNonBlocking(UART0_BASE, ADCstring[3]);
//		UARTCharPutNonBlocking(UART0_BASE, ADCstring[5]);
//}
	
	// 0.1秒钟软定时器计数
	if (++clock100ms >= V_T100ms)
	{
		clock100ms_flag = 1; // 当0.1秒到时，溢出标志置1
		clock100ms = 0;
	}
	
// 	// 0.5秒钟软定时器计数
//	if (++clock500ms >= V_T500ms)
//	{
//		clock500ms_flag = 1; // 当0.5秒到时，溢出标志置1
//		clock500ms = 0;
//	}
	
	// 刷新全部数码管和LED指示灯
	TM1638_RefreshDIGIandLED(digit, pnt, led);

	// 检查当前键盘输入，0代表无键操作，1-8表示有对应按键
	// 键号显示在一位数码管上
	key_code = TM1638_Readkeyboard();
	digit[1] = key_code;
}


//********************************* ADC **********************************************
void ADC0Init(void)
{

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_1 );


    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH2 | ADC_CTL_IE | ADC_CTL_END);

    ADCSequenceEnable(ADC0_BASE, 3);

//	       ADCIntEnable(ADC0_BASE,3);        //使能ADC采样序列的中断
//        IntEnable(INT_ADC0SS0_TM4C129);                 // 使能ADC采样序列中断


    ADCIntClear(ADC0_BASE, 3);
}

uint32_t ADC_Sample(void)
{

    // pui32ADC0Value数组用于从ADC FIFO读取的数据
    uint32_t pui32ADC0Value[1];

    // 触发ADC采样
    ADCProcessorTrigger(ADC0_BASE, 3);

    // 等待采样转换完成
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // 清除ADC中断标志
    ADCIntClear(ADC0_BASE, 3);

    // 读取ADC采样值,存储到数组pui32ADC0Value中
    ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

    // pui32ADC0Value[0]存放了PE1引脚输入电压的采样值
    return pui32ADC0Value[0];
}

uint32_t ADC_sinSample(uint8_t keyvalue)
{
    uint32_t sample[50] = {0};
    uint32_t ret = 0;
    uint32_t i = 0;
    uint32_t j = 0;
    if(keyvalue == 0 && ui8KeyState == 1)
    {
        ui8KeyState = 0;
        for(i = 0; i < 50; i++)
        {
            sample[i] = ADC_Sample();
            delay_usec(20);
        }
        for(j = 0; j < 50; j++)
            if(sample[j] > ret) ret = sample[j];
        ADC0value1 = ret;
        ADCstring[0] = ADC0value1 / 1000 + '0';
        ADCstring[1] = ADC0value1 % 1000 / 100 + '0';
        ADCstring[2] = ADC0value1 % 100 / 10 + '0';
        ADCstring[3] = ADC0value1 % 10 + '0';
        UARTCharPutNonBlocking(UART0_BASE, ADCstring[0]);
        UARTCharPutNonBlocking(UART0_BASE, ADCstring[1]);
        UARTCharPutNonBlocking(UART0_BASE, ADCstring[2]);
        UARTCharPutNonBlocking(UART0_BASE, ADCstring[3]);
        UARTCharPutNonBlocking(UART0_BASE, ADCstring[5]);
    }
    else if(keyvalue == 1 && ui8KeyState == 0)
    {
        ui8KeyState = 1;
    }

    return ret;
}

//***************************************** AD9850 **************************************
void SentFreq()
{
    uint8_t i;
    uint32_t temp;
    AD9850_FQ_L;
    for(i=0; i<32; i++) //串口数据 输入频率控制字
    {
        AD9850_WCLK_L;
        temp=( ( freq >> i ) & 1 );
        if(temp)
        {
            AD9850_D7_H;
        }
        else
        {
            AD9850_D7_L;
        }

        AD9850_WCLK_H;
    }
    for(i=0; i<8; i++)
//phase-b4 ph-b3 ph-b2 ph-b1 ph-b0
    {
        AD9850_WCLK_L;
        temp=( ( Control_AD9850 >> i ) & 1 );
        if(temp)
        {
            AD9850_D7_H;
        }
        else
        {
            AD9850_D7_L;
        }
        AD9850_WCLK_H;
    }
    AD9850_WCLK_L;
    AD9850_FQ_H;
    AD9850_FQ_L;
}

void Set_Freq(uint32_t Freqency)
{
    freq= (uint32_t)(34.36*Freqency);
// SYSCLK = 125 MHz 2^32/125000000=34.36
    SentFreq();
}

//*********************************delay*****************************
void delay_sys(uint32_t delayunit)
{
    while(delayunit--);
}
void delay_usec(uint32_t _usec)
{
    uint32_t loop = usec * _usec;
    while(loop--);
}

