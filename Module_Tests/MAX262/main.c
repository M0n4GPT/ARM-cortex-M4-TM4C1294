
#ifndef TARGET_IS_BLIZZARD_RA1
#define TARGET_IS_BLIZZARD_RA1
#endif

#ifndef  PART_TM4C123GH6PM
#define  PART_TM4C123GH6PM
#endif

/*****************************************************
 * PWM_Tiva ����4��PWM�źţ�
 * 4��PWM ÿ·�ź���λ������90���ƫ�ơ�
 * ��·PWM�źŵ�ռ�ձ��ȶ���25%
 * ***************************************************/

#include <stdbool.h>
#include <stdint.h>
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "max262.h"



//12500 / 20//0x1FFFF
// ���������С����ʱ��   Ƶ����200~1000֮��


/***********************************************************
 * @brief  	������
 *
 * **********************************************************/
//uint32_t index_Statistics = 0;				// Ƶ��ͳ��

int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);

	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//��B��
	  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
	      			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);//PB��Ϊ���

	Init_ADC(ADC_BASE, SYSCTL_PERIPH_GPIOE,
			GPIO_PORTE_BASE, GPIO_PIN_0,
			SequenceNum_ADC_WHEEL, ADC_CTL_CH3);

	Init_LCD_Show();
	Init_PWM();
	//��ʼ����ť
	Init_Int_Key();

	Init_Main();

//	Init_Res_I2C();
	while (1)
	{
		Handler_ADCWheel();

		Handler_Test();

		Refresh_LCDView();
	}
}

void GPIOInit(void)
{
//����MAX262оƬ�ܽ�
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



}
