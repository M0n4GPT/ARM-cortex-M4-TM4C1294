/*
 * max262.h
 *
 *
 */

#ifndef MAX_262_H_
#define MAX_262_H_

#define PI  (3.14159)

//四位地址线
#define Addr0_H     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define Addr0_L     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1,0);
#define Addr1_H     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, GPIO_PIN_2);
#define Addr1_L     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2,0);
#define Addr2_H     GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0, GPIO_PIN_0);
#define Addr2_L     GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_0,0);
#define Addr3_H     GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define Addr3_L     GPIOPinWrite(GPIO_PORTH_BASE, GPIO_PIN_1,0);
//两位数据线
#define Data0_H     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7, GPIO_PIN_7);
#define Data0_L     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_7,0);
#define Data1_H     GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5, GPIO_PIN_5);
#define Data1_L     GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_5,0);
//控制线
#define DataWR_H    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6, GPIO_PIN_6);
#define DataWR_L    GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_6,0);

void MAX262_Write(unsigned char add,unsigned char dat2bit);
void MAX262_SetAF(unsigned char datAF);
void MAX262_SetBF(unsigned char datBF);
void MAX262_SetAQ(unsigned char datAQ);
void MAX262_SetBQ(unsigned char datBQ);
float MAX262_LH_Getf0(float fc,float Q,unsigned char PassMode );
unsigned char MAX262_GetFN(float fclk, float f0,unsigned char WorkMode);
void MAX262_LH(float fc,float Q,unsigned char PassMode,unsigned char WorkMode,unsigned char Channel);
void MAX262_BP(float fh,float fl,unsigned char WorkMode,unsigned char Channel) ;

#endif /* MAX_262_H_ */

