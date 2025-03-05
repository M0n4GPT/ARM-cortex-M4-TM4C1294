//*****************************************************************************

//*****************************************************************************

//*****************************************************************************
//
// ͷ�ļ�
//
//*****************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "define.h"
#include "inc/hw_memmap.h"        // ��ַ�궨��
#include "inc/hw_ints.h"
#include "inc/hw_types.h"         // �������ͺ궨�壬�Ĵ������ʺ���
#include "driverlib/debug.h"      // ������
#include "driverlib/gpio.h"       // ͨ��IO�ں궨��ͺ���ԭ��
#include "driverlib/pin_map.h"    // TM4Cϵ��MCU��Χ�豸�ܽź궨��
#include "driverlib/sysctl.h"	  // ϵͳ���ƶ���
#include "driverlib/systick.h"    // SysTick Driver ԭ��
#include "driverlib/interrupt.h"  // NVIC Interrupt Controller Driver ԭ��
#include "driverlib/uart.h"       // ��UART�йصĺ궨��ͺ���ԭ��
#include "uartstdio.h"       // ��UART�йصĺ궨��ͺ���ԭ��

#include "tm1638.h"               // �����TM1638оƬ�йصĺ궨��ͺ���ԭ��
#include "driverlib/adc.h"
#include "max262.h"               // �����max262оƬ�йصĺ궨��ͺ���ԭ��
//*****************************************************************************
//
// �궨��
//
//*****************************************************************************
#define SYSTICK_FREQUENCY		50		// SysTickƵ��Ϊ50Hz����ѭ����ʱ����20ms

#define V_T100ms	5                  // 0.1s�����ʱ�����ֵ��5��20ms

#define GPIO_PA0_U0RX           0x00000001	//���������궨�壬��֪��Ϊʲôpin_map.h���û�б�ʶ��
#define GPIO_PA1_U0TX           0x00000401
#define usec 5
//*****************************************************************************
//
// ����ԭ������
//
//*****************************************************************************
void GPIOInit(void);        // GPIO��ʼ��
void SysTickInit(void);     // ����SysTick�ж�
void UARTInit(void);        // UART��ʼ��
void DevicesInit(void);     // MCU������ʼ����ע���������������
void UARTStringPut(uint32_t ui32Base,const char *cMessage);// ��UART�����ַ���
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
// ��������
//
//*****************************************************************************

// �����ʱ������
uint8_t clock100ms = 0;
uint8_t clock500ms = 0;


// �����ʱ�������־
uint8_t clock100ms_flag = 0;
uint8_t clock500ms_flag = 0;


// ϵͳʱ��Ƶ��
uint32_t ui32SysClock;
//ADC
uint32_t ADC0value1 = 0;
char ADCstring[6] = "0000\r\n";
uint32_t timeflag = 0;

//AD9850
uint32_t freq = 0;
uint8_t Control_AD9850 = 0x00;
uint8_t debugflag = 0;

//mcu����
uint8_t ui8KeyValue = 1;
uint8_t ui8KeyState = 1;
// tm1638����ֵ
uint8_t key_code = 0;

// 8λ�������ʾ�����ֻ���ĸ����
// ע����������λ�������������Ϊ 0��1��2��3��4��5��6��7
uint8_t digit[8]={'_',' ','_',' ',' ',' ',' ',' '};

// 8λС���� 1��  0��
// ע����������λС����������������Ϊ0��1��2��3��4��5��6��7
uint8_t pnt = 0x00;

// 8��LEDָʾ��״̬��0��1��
// ע������ָʾ�ƴ������������Ϊ0��1��2��3��4��5��6��7
//     ��ӦԪ��LED1��LED2��LED3��LED4��LED5��LED6��LED7��LED8
uint8_t led[] = {1, 0, 0, 0, 0, 0, 0, 0};

//*****************************************************************************
//
//�궨��
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
// ������
//
//*****************************************************************************
int main(void)
{
    uint8_t uart_receive_char;
    DevicesInit();            //  MCU������ʼ��
		
		while (clock100ms < 3);   // ��ʱ>60ms,�ȴ�TM1638�ϵ����
		TM1638_Init();	          // ��ʼ��TM1638
	
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
// ����ԭ�ͣ�void GPIOInit(void)
// �������ܣ�GPIO��ʼ����
//						ʹ��PortL��
//          ��PL0-D7,PL1-FQUD,PL2-WCLK,PL3-RESET��
//						ʹ��PortK������PK4,PK5Ϊ�����ʹ��PortM������PM0Ϊ�����
//          ��PK4����TM1638��STB��PK5����TM1638��DIO��PM0����TM1638��CLK��
// ������������
// ��������ֵ����
//
//*****************************************************************************
void GPIOInit(void)
{
		//////����DDSģ��ܽ�
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);				// ʹ�ܶ˿� L
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)) {};		// �ȴ��˿� L׼�����

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)) {};

    // ���ö˿� L�ĵ�0,1,2,3λΪAD9850���������,PL0-D7,PL1-FQUD,PL2-WCLK,PL3-RESET
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    // ����PJ0
    GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0);
    // PJ0����Ϊ��������
    GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0,GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);

			
		//////����TM1638оƬ�ܽ�
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						// ʹ�ܶ˿� K	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)){};		// �ȴ��˿� K׼�����		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// ʹ�ܶ˿� M	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// �ȴ��˿� M׼�����		
		
		// ���ö˿� K�ĵ�4,5λ��PK4,PK5��Ϊ�������		PK4-STB  PK5-DIO
		GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
		// ���ö˿� M�ĵ�0λ��PM0��Ϊ�������   PM0-CLK
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_0);	
		
		
		//////����MAX262оƬ�ܽ�
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);						// ʹ�ܶ˿� H	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOH)){};		// �ȴ��˿� H׼�����		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);						// ʹ�ܶ˿� M	
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){};		// �ȴ��˿� M׼�����		
		
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);						// ʹ�ܶ˿� P
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP)){};		// �ȴ��˿� P׼�����		
		
		// ���ö˿� H�ĵ�0,1λ��PH0,PH1��Ϊ�������		PH0-A2  PH1-A3
		GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4|GPIO_PIN_5);
		// ���ö˿� M�ĵ�1,2,6,7λ��PM1,2,6,7��Ϊ�������   PM1-A0 PM2-A1 PM6-OE PM7-D0
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_6|GPIO_PIN_7);	
		// ���ö˿� P��5λ��PP5��Ϊ�������   PP5-D1
		GPIOPinTypeGPIOOutput(GPIO_PORTM_BASE, GPIO_PIN_5);	
SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);						// ʹ�ܶ˿� N
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){};		// �ȴ��˿�N׼�����		
		
		// ���ö˿� H�ĵ�0,1λ��PN3��Ϊ�������		PN3-CLK
		GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_3);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, GPIO_PIN_3);
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
// ����ԭ�ͣ�void UARTStringPut(uint32_t ui32Base,const char *cMessage)
// �������ܣ���UARTģ�鷢���ַ���
// ����������ui32Base��UARTģ��
//          cMessage���������ַ���
// ��������ֵ����
//
//*****************************************************************************
void UARTStringPut(uint32_t ui32Base,const char *cMessage)
{
    while(*cMessage != '\0')
        UARTCharPut(ui32Base, *(cMessage++));
}

//*****************************************************************************
//
// ����ԭ�ͣ�void UARTInit(void)
// �������ܣ�UART��ʼ����ʹ��UART0������PA0,PA1ΪUART0 RX,TX���ţ�
//          ���ò����ʼ�֡��ʽ��
// ������������
// ��������ֵ����
//
//*****************************************************************************
void UARTInit(void)
{
    // ��������
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);        // ʹ��UART0ģ��
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        // ʹ�ܶ˿� A
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));	// �ȴ��˿� A׼�����

    GPIOPinConfigure(GPIO_PA0_U0RX);				  // ����PA0ΪUART0 RX����
    GPIOPinConfigure(GPIO_PA1_U0TX);    			  // ����PA1ΪUART0 TX����

    // ���ö˿� A�ĵ�0,1λ��PA0,PA1��ΪUART����
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // �����ʼ�֡��ʽ����
    UARTConfigSetExpClk(UART0_BASE,
                        ui32SysClock,
                        115200,                  // �����ʣ�115200
                        (UART_CONFIG_WLEN_8 |    // ����λ��8
                         UART_CONFIG_STOP_ONE |  // ֹͣλ��1
                         UART_CONFIG_PAR_NONE)); // У��λ����

    // ��ʼ����ɺ���PC�˷���"Hello, 2A!"�ַ���
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
// ����ԭ�ͣ�DevicesInit(void)
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
    UARTInit();             // UART��ʼ��
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
	
	// 0.1������ʱ������
	if (++clock100ms >= V_T100ms)
	{
		clock100ms_flag = 1; // ��0.1�뵽ʱ�������־��1
		clock100ms = 0;
	}
	
// 	// 0.5������ʱ������
//	if (++clock500ms >= V_T500ms)
//	{
//		clock500ms_flag = 1; // ��0.5�뵽ʱ�������־��1
//		clock500ms = 0;
//	}
	
	// ˢ��ȫ������ܺ�LEDָʾ��
	TM1638_RefreshDIGIandLED(digit, pnt, led);

	// ��鵱ǰ�������룬0�����޼�������1-8��ʾ�ж�Ӧ����
	// ������ʾ��һλ�������
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

//	       ADCIntEnable(ADC0_BASE,3);        //ʹ��ADC�������е��ж�
//        IntEnable(INT_ADC0SS0_TM4C129);                 // ʹ��ADC���������ж�


    ADCIntClear(ADC0_BASE, 3);
}

uint32_t ADC_Sample(void)
{

    // pui32ADC0Value�������ڴ�ADC FIFO��ȡ������
    uint32_t pui32ADC0Value[1];

    // ����ADC����
    ADCProcessorTrigger(ADC0_BASE, 3);

    // �ȴ�����ת�����
    while(!ADCIntStatus(ADC0_BASE, 3, false))
    {
    }

    // ���ADC�жϱ�־
    ADCIntClear(ADC0_BASE, 3);

    // ��ȡADC����ֵ,�洢������pui32ADC0Value��
    ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);

    // pui32ADC0Value[0]�����PE1���������ѹ�Ĳ���ֵ
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
    for(i=0; i<32; i++) //�������� ����Ƶ�ʿ�����
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

