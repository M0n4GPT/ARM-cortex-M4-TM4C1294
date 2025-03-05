
//*****************************************************************************

//*****************************************************************************
//
// 头文件
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // 基址宏定义
#include "inc/hw_types.h"         // 数据类型宏定义，寄存器访问函数
#include "driverlib/debug.h"      // 调试用
#include "driverlib/gpio.h"       // 通用IO口宏定义
#include "driverlib/pin_map.h"    // TM4C系列MCU外围设备管脚宏定义
#include "driverlib/sysctl.h"	  // 系统控制定义
#include "driverlib/systick.h"    // SysTick Driver 原型
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver 原型

#include "tm1638.h"               // 与控制TM1638芯片有关的函数
#include "JLX12864.c"
#include <math.h>
#include "8X16.h"
#include "GB1616.h"	//16*16
#include "GB3232.h"	//32*32
//*****************************************************************************
//
// 宏定义
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTick频率为50Hz，即循环定时周期20ms

#define V_T100ms	5              // 0.1s软件定时器溢出值，5个20ms
#define V_T500ms	25             // 0.5s软件定时器溢出值，25个20ms
#define V_T2s  100              //2s软件定时器溢出值，100个20ms


#define White          0xFFFF
#define Black          0x0000
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define TFT_CS_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_5, GPIO_PIN_5)
#define TFT_CS_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_5, 0)
#define TFT_RESET_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define TFT_RESET_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0)
#define TFT_RS_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, GPIO_PIN_1)
#define TFT_RS_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_1, 0)
#define TFT_SDA_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define TFT_SDA_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_2, 0)
#define TFT_SCL_H GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define TFT_SCL_L GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_3, 0)

void delay_sys(uint32_t loop) {
    while(loop--);   //5 = 1us
}

//*****************************************************************************
//
// 函数原型声明
//
//*****************************************************************************
void GPIOInit(void);        // GPIO初始化
void SysTickInit(void);     // 设置SysTick中断
void DevicesInit(void);     // MCU器件初始化，注：会调用上述函数
//*****************************************************************************
//
// 变量定义
//
//*****************************************************************************

// 软件定时器计数
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;
uint8_t clock2s = 0;


// 软件定时器溢出标志
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;
uint8_t clock2s_flag = 0;

// 测试用计数器
uint32_t test_counter = 0;

// 8位数码管显示的数字或字母符号
// 注：板上数码位从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t digit[8]= {' ',' ',' ',' ','_',' ','_',' '};

// 8位小数点 1亮  0灭
// 注：板上数码位小数点从左到右序号排列为4、5、6、7、0、1、2、3
uint8_t pnt = 0x04;

// 8个LED指示灯状态，0灭，1亮
// 注：板上指示灯从左到右序号排列为7、6、5、4、3、2、1、0
//     对应元件LED8、LED7、LED6、LED5、LED4、LED3、LED2、LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// 当前按键值
uint8_t key_code = 0;

// 系统时钟频率
uint32_t ui32SysClock;

//**********************************TFT****************************************
void LCD_PutChar8x16(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor);
void LCD_PutChar(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor);
void LCD_PutString(uint16_t x, uint16_t y,uint8_t *s, uint32_t fColor, uint32_t bColor);

void LCD_PutChar8x8(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor);
void PutGB1616(uint16_t x, uint16_t  y, uint8_t c[2], uint32_t fColor,uint32_t bColor);
void PutGB3232(uint16_t x, uint16_t  y, uint8_t c[2], uint32_t fColor,uint32_t bColor);

uint8_t bitdata;
#define bit7 bitdata&0x80
#define bit6 bitdata&0x40
#define bit5 bitdata&0x20
#define bit4 bitdata&0x10
#define bit3 bitdata&0x08
#define bit2 bitdata&0x04
#define bit1 bitdata&0x02
#define bit0 bitdata&0x01

void  delay(uint32_t t);
uint8_t Zk_ASCII8X16[];

void delay(uint32_t time)
{
    uint32_t i,j;
    for(i=0; i<time; i++)
        for(j=0; j<2500; j++);
}





void LCD_CtrlWrite_IC(uint8_t c)
{
    TFT_CS_L;
    delay_sys(10);

    TFT_RS_L;
    delay_sys(10);
    bitdata=c;


    if(bit7) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit6) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit5) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit4) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit3) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit2) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit1) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit0) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    TFT_CS_H;
    delay_sys(10);

}
void  LCD_DataWrite_IC(uint8_t d)
{
    TFT_CS_L;
    TFT_RS_H;
    bitdata=d;

    if(bit7) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit6) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit5) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit4) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit3) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit2) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit1) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit0) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    TFT_CS_H;
}

void LCD_DataWrite(uint8_t LCD_DataH,uint8_t LCD_DataL)
{
    LCD_DataWrite_IC(LCD_DataH);
    LCD_DataWrite_IC(LCD_DataL);
}
void wr_dat(uint32_t dat)
{
    LCD_DataWrite_IC(dat>>8);

    LCD_DataWrite_IC(dat);
}

void  write_command(uint8_t c)
{
    TFT_CS_L;


    TFT_RS_L;
    bitdata=c;

    if(bit7) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit6) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit5) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit4) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit3) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit2) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit1) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit0) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    TFT_CS_H;
}

void  write_data(uint8_t d)
{
    TFT_CS_L;
    TFT_RS_H;
    bitdata=d;
    if(bit7) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit6) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit5) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit4) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit3) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit2) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit1) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    if(bit0) TFT_SDA_H;
    else TFT_SDA_L;
    TFT_SCL_L;
    TFT_SCL_H;
    TFT_CS_H;
}


void Reset()
{
    TFT_RESET_L;
    delay(100);
    TFT_RESET_H;
    delay(100);
}
//////////////////////////////////////////////////////////////////////////////////////////////

void lcd_initial()
{

    TFT_RESET_L;
    delay(100);
    TFT_RESET_H;
    delay(100);

    //------------------------------------------------------------------//
    //-------------------Software Reset-------------------------------//

    //2.2inch TM2.2-G2.2 Init 20171020
    write_command(0x11);
    write_data(0x00);

    write_command(0xCF);
    write_data(0X00);
    write_data(0XC1);
    write_data(0X30);

    write_command(0xED);
    write_data(0X64);
    write_data(0X03);
    write_data(0X12);
    write_data(0X81);

    write_command(0xE8);
    write_data(0X85);
    write_data(0X11);
    write_data(0X78);

    write_command(0xF6);
    write_data(0X01);
    write_data(0X30);
    write_data(0X00);

    write_command(0xCB);
    write_data(0X39);
    write_data(0X2C);
    write_data(0X00);
    write_data(0X34);
    write_data(0X05);

    write_command(0xF7);
    write_data(0X20);

    write_command(0xEA);
    write_data(0X00);
    write_data(0X00);

    write_command(0xC0);
    write_data(0X20);

    write_command(0xC1);
    write_data(0X11);

    write_command(0xC5);
    write_data(0X31);
    write_data(0X3C);

    write_command(0xC7);
    write_data(0XA9);

    write_command(0x3A);
    write_data(0X55);

    write_command(0x36);
    write_data(0x48); //竖屏参数

    write_command(0xB1);
    write_data(0X00);
    write_data(0X18);

    write_command(0xB4);
    write_data(0X00);
    write_data(0X00);

    write_command(0xF2);
    write_data(0X00);

    write_command(0x26);
    write_data(0X01);

    write_command(0xE0);
    write_data(0X0F);
    write_data(0X17);
    write_data(0X14);
    write_data(0X09);
    write_data(0X0C);
    write_data(0X06);
    write_data(0X43);
    write_data(0X75);
    write_data(0X36);
    write_data(0X08);
    write_data(0X13);
    write_data(0X05);
    write_data(0X10);
    write_data(0X0B);
    write_data(0X08);


    write_command(0xE1);
    write_data(0X00);
    write_data(0X1F);
    write_data(0X23);
    write_data(0X03);
    write_data(0X0E);
    write_data(0X04);
    write_data(0X39);
    write_data(0X25);
    write_data(0X4D);
    write_data(0X06);
    write_data(0X0D);
    write_data(0X0B);
    write_data(0X33);
    write_data(0X37);
    write_data(0X0F);

    write_command(0x29);
}



void  RamAdressSet()
{
    write_command(0x2A);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0xaf);

    write_command(0x2B);
    write_data(0x00);
    write_data(0x00);
    write_data(0x00);
    write_data(0xdb);
}

static void LCD_SetPos(uint32_t x0,uint32_t x1,uint32_t y0,uint32_t y1)
{
    write_command(0x2A);
    write_data(x0>>8);
    write_data(x0);
    write_data(x1>>8);
    write_data(x1);

    write_command(0x2B);
    write_data(y0>>8);
    write_data(y0);
    write_data(y1>>8);
    write_data(y1);
    write_command(0x2c);
}

void LCD_PutChar(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor) {


    LCD_PutChar8x16( x, y, c, fColor, bColor );
}

void LCD_PutString(uint16_t x, uint16_t y, uint8_t *s, uint32_t fColor, uint32_t bColor) {
    uint8_t l=0;
    while(*s) {
        if( *s < 0x80)
        {
            LCD_PutChar(x+l*8,y,*s,fColor,bColor);
            s++;
            l++;
        }
        else
        {
            PutGB1616(x+l*8,y,(uint8_t*)s,fColor,bColor);
            s+=2;
            l+=2;
        }
    }
}


void LCD_drawPoint(uint16_t x, uint16_t y, uint32_t fColor)
{
    LCD_SetPos(x,x,y,y);
    wr_dat(fColor);
}

void LCD_drawLine(int x1,int y1,int x2,int y2,uint32_t fColor)
{

    uint32_t i;
    int xstep;
    int ystep;
    int prex = x1 * 240;
    int prey = y1 * 320;
    int step = abs(x2-x1) > abs(y2-y1) ? abs(x2-x1) : abs(y2-y1);
    xstep = (x2-x1) * 240 / step;
    ystep = (y2-y1) * 320 / step;
    for(i = 0 ; i < step ; i++)
    {
        prex += xstep;
        prey += ystep;
        LCD_drawPoint(prex/240 , prey/320 , fColor);
    }

}

void LCD_PutChar8x16(uint16_t x, uint16_t y, uint8_t c, uint32_t fColor, uint32_t bColor)
{
    uint32_t i,j;
    LCD_SetPos(x,x+8-1,y,y+16-1);
    for(i=0; i<16; i++) {
        uint8_t m=Font8x16[c*16+i];
        for(j=0; j<8; j++) {
            if((m&0x80)==0x80) {
                wr_dat(fColor);
            }
            else {
                wr_dat(bColor);
            }
            m<<=1;
        }
    }
}

void PutGB1616(uint16_t x, uint16_t  y, uint8_t c[2], uint32_t fColor,uint32_t bColor) {

    uint32_t i,j,k;

    LCD_SetPos(x,  x+16-1,y, y+16-1);

    for (k=0; k<64; k++) {
        if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1])) {
            for(i=0; i<32; i++) {
                uint16_t m=codeGB_16[k].Msk[i];
                for(j=0; j<8; j++) {
                    if((m&0x80)==0x80) {
                        wr_dat(fColor);
                    }
                    else {
                        wr_dat(bColor);
                    }
                    m<<=1;
                }
            }
        }
    }
}

void PutGB3232(uint16_t x, uint16_t  y, uint8_t c[2], uint32_t fColor,uint32_t bColor) {
    uint32_t i,j,k;

    LCD_SetPos(x,  x+32-1,y, y+32-1);

    for (k=0; k<15; k++) { //15???????????,??????
        if ((codeGB_32[k].Index[0]==c[0])&&(codeGB_32[k].Index[1]==c[1])) {
            for(i=0; i<128; i++) {
                uint16_t m=codeGB_32[k].Msk[i];
                for(j=0; j<8; j++) {
                    if((m&0x80)==0x80) {
                        wr_dat(fColor);
                    }
                    else {
                        wr_dat(bColor);
                    }
                    m<<=1;
                }
            }
        }
    }
}


void dsp_single_colour(uint8_t DH,uint8_t DL)
{
    uint32_t i,j;
    LCD_SetPos(0,240-1,0,320-1);
    for (i=0; i<240; i++)
        for (j=0; j<320; j++)
            LCD_DataWrite(DH,DL);
}
//******************************************************************************


//*****************************************************************************
//
// 主程序
//
//*****************************************************************************
int main(void)
{

    DevicesInit();            //  MCU器件初始化

    lcd_initial();
    LCD_drawLine(5,5,100,100,0xF800);

    while (1)
    {
        write_command(0x2C);
//			dsp_single_colour(0xF8,0x00);
//			LCD_PutString(2,20,"http://qdtech.taobao.com/",Red,Blue);                  ///前景颜色和背景颜色，可以直接预定义，也可以直接填入16进制数字
//			LCD_PutString(2,40,"I LOVE MY JOB ",Blue,Blue2);
//			LCD_PutString(2,60,"全动电子科技有限公司",Magenta,Green);
//			LCD_PutString(2,80,"专业技术支持论坛",0x07e0,0xf800);
//			LCD_PutString(2,100,"专业开发板工控板",0xF800,Blue2);
//			LCD_PutString(2,120,"1234567890",0xF800,Blue);
//			LCD_PutString(2,140,"ASDFGHJKL",0xF800,Magenta);
//			PutGB3232(10,180,"全",Blue,0X00);
//			PutGB3232(50,180,"动",Blue,0X00);
//			PutGB3232(90,180,"电",Blue,0X00);
//			PutGB3232(130,180,"子",Blue,0X00);

//			delay(1000);
//			PutGB3232(10,180,"全",0xF800,0xF800);
//			PutGB3232(50,180,"动",0xF800,0xF800);
//			PutGB3232(90,180,"电",0xF800,0xF800);
//			PutGB3232(130,180,"子",0xF800,0xF800);
//			delay(50);

    }

}

//*****************************************************************************
//
// 函数原型：void GPIOInit(void)
// 函数功能：GPIO初始化。使能PortK，设置PK4,PK5为输出；使能PortM，设置PM0为输出。
//          （PK4连接TM1638的STB，PK5连接TM1638的DIO，PM0连接TM1638的CLK）
// 函数参数：无
// 函数返回值：无
//
//*****************************************************************************
void GPIOInit(void)
{
    //配置TM1638芯片管脚
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// 使能端口 K
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {};		// 等待端口 K准备完毕

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// 使能端口 M
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)) {};		// 等待端口 M准备完毕

    // 设置端口 K的第4,5位（PK4,PK5）为输出引脚		PK4-STB  PK5-DIO
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
    // 设置端口 M的第0位（PM0）为输出引脚   PM0-CLK
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);				// 使能端口 N
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {};		// 等待端口N准备完
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2|GPIO_PIN_3);

    //TFT引脚
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);						// 使能端口 L
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {};		// 等待端口 L准备完毕

    // 设置端口 L的第4,5位（PK4,PK5）为输出引脚		PL5-CS	PL0-RST 	PL1-RS	 PL2-SDI 	 PL3-SCK
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    //SDO引脚可以不连接
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
// 函数原型：void DevicesInit(void)
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
    // 0.1秒钟软定时器计数
    if (++clock100ms >= V_T100ms)
    {
        clock100ms_flag = 1; // 当0.1秒到时，溢出标志置1
        clock100ms = 0;
    }

    // 0.5秒钟软定时器计数
    if (++clock500ms >= V_T500ms)
    {
        clock500ms_flag = 1; // 当0.5秒到时，溢出标志置1
        clock500ms = 0;
    }

    // 2秒钟软中断定时计数器
    if (++clock2s >= V_T2s)
    {
        clock2s_flag =1;  // 当2秒到时，溢出标志置1
        clock2s = 0;
    }

    // 刷新全部数码管和LED指示灯
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // 检查当前键盘输入，0代表无键操作，1-9表示有对应按键
    // 键号显示在一位数码管上
    key_code = TM1638_Readkeyboard();

    digit[5] = key_code;
}
