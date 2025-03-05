
#ifndef TARGET_IS_BLIZZARD_RA1
#define TARGET_IS_BLIZZARD_RA1
#endif

#ifndef  PART_TM4C123GH6PM
#define  PART_TM4C123GH6PM
#endif

/*****************************************************
 * PWM_Tiva 生成4个PWM信号，
 * 4个PWM 每路信号相位正好有90°的偏移。
 * 四路PWM信号的占空比稳定在25%
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
// 定义最大最小周期时间   频率在200~1000之间


/***********************************************************
 * @brief  	主函数
 *
 * **********************************************************/
//uint32_t index_Statistics = 0;				// 频率统计

int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);

	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//开B口
	  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|
	      			GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);//PB设为输出

	Init_ADC(ADC_BASE, SYSCTL_PERIPH_GPIOE,
			GPIO_PORTE_BASE, GPIO_PIN_0,
			SequenceNum_ADC_WHEEL, ADC_CTL_CH3);

	Init_LCD_Show();
	Init_PWM();
	//初始化按钮
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
//配置MAX262芯片管脚
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



}
