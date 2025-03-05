
//*****************************************************************************

//*****************************************************************************
//
// ͷ�ļ�
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��

#include "tm1638.h"               // �����TM1638оƬ�йصĺ���
#include "JLX12864.c"
#include <math.h>
#include "8X16.h"
#include "GB1616.h"	//16*16
#include "GB3232.h"	//32*32
//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T100ms	5              // 0.1s�����ʱ�����ֵ��5��20ms
#define V_T500ms	25             // 0.5s�����ʱ�����ֵ��25��20ms
#define V_T2s  100              //2s�����ʱ�����ֵ��100��20ms


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
// ����ԭ������
//
//*****************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж�
void DevicesInit(void);     // MCU������ʼ����ע���������������
//*****************************************************************************
//
// ��������
//
//*****************************************************************************

// �����ʱ������
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;
uint8_t clock2s = 0;


// �����ʱ�������־
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;
uint8_t clock2s_flag = 0;

// �����ü�����
uint32_t test_counter = 0;

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ4��5��6��7��0��1��2��3
uint8_t digit[8]= {' ',' ',' ',' ','_',' ','_',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ4��5��6��7��0��1��2��3
uint8_t pnt = 0x04;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ7��6��5��4��3��2��1��0
//     ��ӦԪ��LED8��LED7��LED6��LED5��LED4��LED3��LED2��LED1
uint8_t led[] = {1, 1, 1, 1, 1, 1, 1, 0};

// ��ǰ����ֵ
uint8_t key_code = 0;

// ϵͳʱ��Ƶ��
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
    write_data(0x48); //��������

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
// ������
//
//*****************************************************************************
int main(void)
{

    DevicesInit();            //  MCU������ʼ��

    lcd_initial();
    LCD_drawLine(5,5,100,100,0xF800);

    while (1)
    {
        write_command(0x2C);
//			dsp_single_colour(0xF8,0x00);
//			LCD_PutString(2,20,"http://qdtech.taobao.com/",Red,Blue);                  ///ǰ����ɫ�ͱ�����ɫ������ֱ��Ԥ���壬Ҳ����ֱ������16��������
//			LCD_PutString(2,40,"I LOVE MY JOB ",Blue,Blue2);
//			LCD_PutString(2,60,"ȫ�����ӿƼ����޹�˾",Magenta,Green);
//			LCD_PutString(2,80,"רҵ����֧����̳",0x07e0,0xf800);
//			LCD_PutString(2,100,"רҵ�����幤�ذ�",0xF800,Blue2);
//			LCD_PutString(2,120,"1234567890",0xF800,Blue);
//			LCD_PutString(2,140,"ASDFGHJKL",0xF800,Magenta);
//			PutGB3232(10,180,"ȫ",Blue,0X00);
//			PutGB3232(50,180,"��",Blue,0X00);
//			PutGB3232(90,180,"��",Blue,0X00);
//			PutGB3232(130,180,"��",Blue,0X00);

//			delay(1000);
//			PutGB3232(10,180,"ȫ",0xF800,0xF800);
//			PutGB3232(50,180,"��",0xF800,0xF800);
//			PutGB3232(90,180,"��",0xF800,0xF800);
//			PutGB3232(130,180,"��",0xF800,0xF800);
//			delay(50);

    }

}

//*****************************************************************************
//
// ����ԭ�ͣ�void GPIOInit(void)
// �������ܣ�GPIO��ʼ����ʹ��PortK������PK4,PK5Ϊ�����ʹ��PortM������PM0Ϊ�����
//          ��PK4����TM1638��STB��PK5����TM1638��DIO��PM0����TM1638��CLK��
// ������������
// ��������ֵ����
//
//*****************************************************************************
void GPIOInit(void)
{
    //����TM1638оƬ�ܽ�
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// ʹ�ܶ˿� K
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)) {};		// �ȴ��˿� K׼�����

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// ʹ�ܶ˿� M
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)) {};		// �ȴ��˿� M׼�����

    // ���ö˿� K�ĵ�4,5λ��PK4,PK5��Ϊ�������		PK4-STB  PK5-DIO
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
    // ���ö˿� M�ĵ�0λ��PM0��Ϊ�������   PM0-CLK
    GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);				// ʹ�ܶ˿� N
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)) {};		// �ȴ��˿�N׼����
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_2|GPIO_PIN_3);

    //TFT����
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);						// ʹ�ܶ˿� L
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {};		// �ȴ��˿� L׼�����

    // ���ö˿� L�ĵ�4,5λ��PK4,PK5��Ϊ�������		PL5-CS	PL0-RST 	PL1-RS	 PL2-SDI 	 PL3-SCK
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    //SDO���ſ��Բ�����
}

//*****************************************************************************
//
// ����ԭ�ͣ�SysTickInit(void)
// �������ܣ�����SysTick�ж�
// ������������
// ��������ֵ����
//
//*****************************************************************************
void SysTickInit(void)
{
    SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); // ������������,��ʱ����20ms
    SysTickEnable();  			// SysTickʹ��
    SysTickIntEnable();			// SysTick�ж�����
}

//*****************************************************************************
//
// ����ԭ�ͣ�void DevicesInit(void)
// �������ܣ�CU������ʼ��������ϵͳʱ�����á�GPIO��ʼ����SysTick�ж�����
// ������������
// ��������ֵ����
//
//*****************************************************************************
void DevicesInit(void)
{
    // ʹ���ⲿ25MHz��ʱ��Դ������PLL��Ȼ���ƵΪ20MHz
    ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480),
                                      20000000);

    GPIOInit();             // GPIO��ʼ��
    SysTickInit();          // ����SysTick�ж�
    IntMasterEnable();			// ���ж�����
}

//*****************************************************************************
//
// ����ԭ�ͣ�void SysTick_Handler(void)
// �������ܣ�SysTick�жϷ������
// ������������
// ��������ֵ����
//
//*****************************************************************************
void SysTick_Handler(void)       // ��ʱ����Ϊ20ms
{
    // 0.1������ʱ������
    if (++clock100ms >= V_T100ms)
    {
        clock100ms_flag = 1; // ��0.1�뵽ʱ�������־��1
        clock100ms = 0;
    }

    // 0.5������ʱ������
    if (++clock500ms >= V_T500ms)
    {
        clock500ms_flag = 1; // ��0.5�뵽ʱ�������־��1
        clock500ms = 0;
    }

    // 2�������ж϶�ʱ������
    if (++clock2s >= V_T2s)
    {
        clock2s_flag =1;  // ��2�뵽ʱ�������־��1
        clock2s = 0;
    }

    // ˢ��ȫ������ܺ�LEDָʾ��
    TM1638_RefreshDIGIandLED(digit, pnt, led);

    // ��鵱ǰ�������룬0�����޼�������1-9��ʾ�ж�Ӧ����
    // ������ʾ��һλ�������
    key_code = TM1638_Readkeyboard();

    digit[5] = key_code;
}
