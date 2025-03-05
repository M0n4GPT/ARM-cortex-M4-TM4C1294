//*****************************************************************************
// 
// Description: 
//    1. After power-on or reset, the rightmost four digits of the display on the baseboard automatically show the timing value, with the least significant digit representing 0.1 seconds;
//    2. After power-on or reset, the 8 LEDs on the baseboard run in a "chase" pattern, changing from left to right, with a cycle time of approximately 0.5 seconds per change;
//    3. When no button is pressed, the second digit from the left on the display shows "0";
//      When a button is pressed, the display shows the corresponding button number;
//      At this moment, the four-digit timer pauses and stops counting, resuming once the button is released.
// 
//
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
//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T100ms	5              // 0.1s�����ʱ�����ֵ��5��20ms
#define V_T500ms	25             // 0.5s�����ʱ�����ֵ��25��20ms
#define V_T2s  100              //2s�����ʱ�����ֵ��100��20ms

//*********************************PCF8591*****************************************
#define PCF8591_SCL_H GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define PCF8591_SCL_L	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, 0)
#define PCF8591_SDA_H	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define PCF8591_SDA_L GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_2, 0)
#define PCF8591_ADDR_WR 0x90
#define PCF8591_CONTROL 0x40


void delay_sys(uint32_t loop){while(loop--);}

void start()
  {
   PCF8591_SCL_H;
   delay_sys(5);
   PCF8591_SDA_H;
   delay_sys(25);
   PCF8591_SDA_L;
	 delay_sys(25);
		PCF8591_SCL_L;
		delay_sys(10);
  }

void stop()
  {
   PCF8591_SCL_L;
   PCF8591_SDA_L;
   delay_sys(25);
		PCF8591_SCL_H;
		delay_sys(25);
   PCF8591_SDA_H;
		delay_sys(25);
   }

void ack()
{
  PCF8591_SDA_L;
	delay_sys(25);	
  PCF8591_SCL_H;
  delay_sys(25);
  PCF8591_SCL_L;
	delay_sys(25);
}

void noAck()
{
  PCF8591_SDA_H;
  PCF8591_SCL_H;
  delay_sys(25);
  PCF8591_SCL_L;
}

void send(uint8_t Data)
{
   uint8_t i=0;
   uint8_t temp=0;
   temp = Data;
   for(i=0;i<8;i++)
   {
     PCF8591_SCL_L;
     delay_sys(25);
     if(temp&0x80)PCF8591_SDA_H;
         else PCF8591_SDA_L;
         delay_sys(25);
         PCF8591_SCL_H;
         delay_sys(25);
         temp = temp << 1;
					delay_sys(25);
   }
   PCF8591_SCL_L;
	 delay_sys(25);
}

void DAC(uint8_t light)
{
PCF8591_SCL_L;
delay_sys(25);
start();
delay_sys(25);
send(PCF8591_ADDR_WR);
ack();
delay_sys(25);
send(0x40);
ack();
delay_sys(25);	
send(light);
ack();
stop();
delay_sys(500);
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

//LCD��Ļ״̬��
uint8_t lcd_act = 0;

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

//*****************************************************************************
//
// ������
//
//*****************************************************************************
 int main(void)
{
		uint8_t tmp = 0;
    DevicesInit();            //  MCU������ʼ��

    while (clock100ms < 3);   // ��ʱ>60ms,�ȴ�TM1638�ϵ����
    TM1638_Init();	          // ��ʼ��TM1638

    initial_lcd();
		DAC(0x80);
    while (1)
    {
				
					
//			PCF8591_SCL_H;
//			delay_sys(25);
//			PCF8591_SCL_L;
//			delay_sys(25);
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
