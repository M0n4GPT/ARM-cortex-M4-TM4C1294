/*
 * MAX_262.c
 *
 *
 *
 */
#ifndef TARGET_IS_BLIZZARD_RA1
#define TARGET_IS_BLIZZARD_RA1
#endif

#ifndef  PART_TM4C123GH6PM
#define  PART_TM4C123GH6PM
#endif
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/ssi.h"
#include "max262.h"

/*********************************************************************
函数名称：MAX262_Write()
函数功能：对MAX262进行写值，为后面的参数配置提供入口
入口参数：待写入地址、带写入值
出口参数：无
函数说明：利用Pb口作为接口、高位到低位依次为A3/A2/A1/A0/D1/D0/WR/空位
          值每次只能写两位
**********************************************************************/
void MAX262_Write(unsigned char add,unsigned char dat2bit)
{
	switch(add)
	  {
	    case 0:{Addr3_L;Addr2_L;Addr1_L;Addr0_L;break;}
	    case 1:{Addr3_L;Addr2_L;Addr1_L;Addr0_H;break;}
	    case 2:{Addr3_L;Addr2_L;Addr1_H;Addr0_L;break;}
	    case 3:{Addr3_L;Addr2_L;Addr1_H;Addr0_H;break;}
	    case 4:{Addr3_L;Addr2_H;Addr1_L;Addr0_L;break;}
	    case 5:{Addr3_L;Addr2_H;Addr1_L;Addr0_H;break;}
	    case 6:{Addr3_L;Addr2_H;Addr1_H;Addr0_L;break;}
	    case 7:{Addr3_L;Addr2_H;Addr1_H;Addr0_H;break;}
	    case 8:{Addr3_H;Addr2_L;Addr1_L;Addr0_L;break;}
	    case 9:{Addr3_H;Addr2_L;Addr1_L;Addr0_H;break;}
	    case 10:{Addr3_H;Addr2_L;Addr1_H;Addr0_L;break;}
	    case 11:{Addr3_H;Addr2_L;Addr1_H;Addr0_H;break;}
	    case 12:{Addr3_H;Addr2_H;Addr1_L;Addr0_L;break;}
	    case 13:{Addr3_H;Addr2_H;Addr1_L;Addr0_H;break;}
	    case 14:{Addr3_H;Addr2_H;Addr1_H;Addr0_L;break;}
	    case 15:{Addr3_H;Addr2_H;Addr1_H;Addr0_H;break;}
	    default:{Addr3_L;Addr2_L;Addr1_L;Addr0_L;break;}
	  }
	  DataWR_L;
	  SysCtlDelay (SysCtlClockGet() / 3000);
	  switch(dat2bit)
	  {
	    case 0:{Data1_L;Data0_L;break;}
	    case 1:{Data1_L;Data0_H;break;}
	    case 2:{Data1_H;Data0_L;break;}
	    case 3:{Data1_H;Data0_H;break;}
	    default:{Data1_L;Data0_L;break;}
	  }
	  SysCtlDelay (SysCtlClockGet() / 3000);
	  DataWR_H;
	  SysCtlDelay (SysCtlClockGet() / 3000);

}
/*********************************************************************
函数名称：MAX262_SetAF()
函数功能：对MAX262 AFN值写入
入口参数：带写入值
出口参数：无
函数说明：F值有6位，分三次写完  地址分别为1、2、3
**********************************************************************/
void MAX262_SetAF(unsigned char datAF)
{
  MAX262_Write(1,datAF);
  datAF>>=2;
  MAX262_Write(2,datAF);
  datAF>>=2;
  MAX262_Write(3,datAF);

}

/*********************************************************************
函数名称：MAX262_SetBF()
函数功能：对MAX262 BFN值写入
入口参数：带写入值
出口参数：无
函数说明：F值有6位，分三次写完 地址分别为9、10、11
**********************************************************************/
void MAX262_SetBF(unsigned char datBF)
{
  MAX262_Write(9,datBF);
  datBF>>=2;
  MAX262_Write(10,datBF);
  datBF>>=2;
  MAX262_Write(11,datBF);

}
/*********************************************************************
函数名称：MAX262_SetAQ()
函数功能：对MAX262 AQN值写入
入口参数：带写入值
出口参数：无
函数说明：Q值有7位，分四次次写完 地址分别为 4、5、6、7
**********************************************************************/
void MAX262_SetAQ(unsigned char datAQ)
{
  MAX262_Write(4,datAQ);
  datAQ>>=2;
  MAX262_Write(5,datAQ);
  datAQ>>=2;
  MAX262_Write(6,datAQ);
  datAQ>>=2;
  datAQ &=0X01;
  MAX262_Write(7,datAQ);
}

/*********************************************************************
函数名称：MAX262_SetBQ()
函数功能：对MAX262 BQN值写入
入口参数：带写入值
出口参数：无
函数说明：Q值有7位，分四次次写完 地址分别为 12、13、14、15
**********************************************************************/
void MAX262_SetBQ(unsigned char datBQ)
{
  MAX262_Write(12,datBQ);
  datBQ>>=2;
  MAX262_Write(13,datBQ);
  datBQ>>=2;
  MAX262_Write(14,datBQ);
  datBQ>>=2;
  datBQ &=0X01;
  MAX262_Write(15,datBQ);
}

/*********************************************************************
函数名称：MAX262_LH_Getf0()
函数功能：根据外部输入的fc和Q值确定中心频率f0
入口参数：截止频率fc，品质因数Q，滤波模式
出口参数：中心频率f0
函数说明：仅适用于低通和高通
**********************************************************************/
float MAX262_LH_Getf0(float fc,float Q,unsigned char PassMode )
{
  float Temp;
  Temp = 1-0.5/pow(Q,2);
  Temp = sqrt(Temp+sqrt(pow(Temp,2)+1));
  if(PassMode == highPass)
  {
    Temp = fc*Temp;

  }
  else
    if(PassMode == lowPass)
    {
      Temp = fc/Temp;
    }
  return Temp;

}
/*********************************************************************
函数名称：MAX262_GetFN
函数功能：根据中心频率f0、max262系统时钟及其工作模式确定FN值
入口参数：max262系统时钟fclk，中心频率f0， 工作模式WorkMode
出口参数：FN值
函数说明：
**********************************************************************/
unsigned char MAX262_GetFN(float fclk, float f0,unsigned char WorkMode)
{
  unsigned char FN;
  if(WorkMode == modeTwo)
    FN = (unsigned char)(fclk/f0/1.11072-26);
  else
    FN = (unsigned char)(fclk/f0/PI*2-26);
  return FN;

}
/*********************************************************************
函数名称：MAX262_LH
函数功能：配置低通（高通）
入口参数：截止频率fc，品质因数Q，滤波模式，工作模式WorkMode，通道
出口参数：无
函数说明：适用于低通和高通
**********************************************************************/
void MAX262_LH(float fc,float Q,unsigned char PassMode,unsigned char WorkMode,unsigned char Channel)
{
  float f0;
  unsigned char QN;
  unsigned char DIV;

  f0 = MAX262_LH_Getf0(fc,Q,PassMode );
  if(f0>97000||f0<8)
    f0=10000;
  if(Q<0.5)
    Q=1;
  if(WorkMode == modeTwo)
    QN = (unsigned char)(128-90.51/Q);
  else
    QN = (unsigned char)(128-64/Q);
   if(PassMode==highPass)
	      WorkMode=modeThree;	 //高通只有工作模式3
      if(Channel==channelA)
      {
        MAX262_Write(0,WorkMode);
        MAX262_SetAQ(QN);
      }
    else
      {
        MAX262_Write(8,WorkMode);
        MAX262_SetBQ(QN);
      }
      if(f0>28612)
        DIV = 1;    //1分频 smclk 4M fclk4M
      else
        if(f0>14306)
          DIV = 2;  //4分频 smclk 2M fclk 2M
            else
              if(f0>7153)
                DIV = 3;  //8分频 smclk 1M fclk 1M
                  else
                    if(f0>3576)
                      DIV = 4;   //16分频 smclk 500K fclk 500k
                        else
                          if(f0>1788)
                            DIV = 5;  //32分频 SMCLK 250K fclk 250K
                            else
                              if(f0>894)

                                DIV = 6;                //SMCLK 为125K
                            else
                              if(f0>447)
                                DIV = 7;              //SMCLK 为62.5K
                              else
                                if(f0>234)
                                  DIV = 8;          //SMCLK 为32768HZ
                                else
                                  if(f0>117)
                                    DIV = 9;       //SMCLK为16384HZ
                                else
                                  if(f0>58)
                                    DIV = 10;       //SMCLK为 8194HZ
                                else
                                  if(f0>29)
                                    DIV = 11;      //SMCLK 为4096HZ
                                else
                                  if(f0>14)
                                    DIV = 12;      //SMCLK 为2048HZ
                                else
                                  if(f0>7 )
                                    DIV = 13;        //SMCLK 我1024HZ

    switch(DIV)
    {


      case 1:
             if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(4000000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(4000000,f0,WorkMode));

             break;
      case 2:
               if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(2000000,f0,WorkMode));
               else
                MAX262_SetBF( MAX262_GetFN(2000000,f0,WorkMode));

             break;
      case 3:
               if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(1000000,f0,WorkMode));
               else
                MAX262_SetBF( MAX262_GetFN(1000000,f0,WorkMode));
               break;


      case 4:
              if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(500000,f0,WorkMode));
              else
                MAX262_SetBF( MAX262_GetFN(500000,f0,WorkMode));

             break;

      case 5:
              if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(250000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(250000,f0,WorkMode));
             break;
      case 6:if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(125000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(125000,f0,WorkMode));
             break;
      case 7:
    	      if(Channel==channelA)
    	  	  	  MAX262_SetAF( MAX262_GetFN(62500,f0,WorkMode));
       	   	   else
       	   		   MAX262_SetBF( MAX262_GetFN(62500,f0,WorkMode));
      	  	  break;

      case 8: if(Channel==channelA)
  	  	      MAX262_SetAF( MAX262_GetFN(32768,f0,WorkMode));
	   	      else
	   		  MAX262_SetBF( MAX262_GetFN(32768,f0,WorkMode));
	  	     break;
      case 9: if(Channel==channelA)
  	  	       MAX262_SetAF( MAX262_GetFN(16384,f0,WorkMode));
	   	      else
	   		   MAX262_SetBF( MAX262_GetFN(16384,f0,WorkMode));
	  	       break;
      case 10: if(Channel==channelA)
  	  	       MAX262_SetAF( MAX262_GetFN(8192,f0,WorkMode));
	   	      else
	   		   MAX262_SetBF( MAX262_GetFN(8192,f0,WorkMode));
	  	       break;
      case 11: if(Channel==channelA)
  	  	       MAX262_SetAF( MAX262_GetFN(4096,f0,WorkMode));
	   	       else
	   		   MAX262_SetBF( MAX262_GetFN(4096,f0,WorkMode));
	  	       break;
      case 12: if(Channel==channelA)
       	  	  MAX262_SetAF( MAX262_GetFN(2048,f0,WorkMode));
     	   	   else
     	   		   MAX262_SetBF( MAX262_GetFN(2048,f0,WorkMode));
     	  	  break;
      case 13: if(Channel==channelA)
       	  	  MAX262_SetAF( MAX262_GetFN(1024,f0,WorkMode));
     	   	   else
     	   		   MAX262_SetBF( MAX262_GetFN(1024,f0,WorkMode));
     	  	  break;
      default: break;
    }

    SMCLK_Configure(DIV);
}
/**************************************************************************
函数名称：MAX262_BP
函数功能：配置带通
入口参数：上限截止频率fh，下限截止频率fl，工作模式WorkMode，通道
出口参数：无
函数说明：适用于带通,传参时注意第一个参数是上限截止频率，第二个是下限截止频率

*****************************************************************************/

void MAX262_BP(float fh,float fl,unsigned char WorkMode,unsigned char Channel)
{
  float f0;
  float Q;
  unsigned char QN;
  unsigned char DIV;
  f0 = sqrt(fh*fl);
  Q = f0/(fh-fl);

 if (WorkMode==modeTwo)
      QN = (unsigned char)(128-90.51/Q);
  else
      QN = (unsigned char)(128-64/Q);
    if(Channel==channelA)
      {
        MAX262_Write(0,WorkMode);
        MAX262_SetAQ(QN);
      }
    else
      {
        MAX262_Write(8,WorkMode);
        MAX262_SetBQ(QN);
      }

      if(f0>28612)
        DIV = 1;    //2分频 smclk 4M fclk4M
      else
        if(f0>14306)
          DIV = 2;  //4分频 smclk 2M fclk 2M
            else
              if(f0>7153)
                DIV = 3;  //8分频 smclk 1M fclk 1M
                  else
                    if(f0>3576)
                      DIV = 4;   //16分频 smclk 500K fclk 500k
                        else
                          if(f0>1788)
                            DIV = 5;  //32分频 smclk 250K fclk 250K
                            else
                              if(f0>894)
                               DIV = 6;                //SMCLK 为125K
                            else
                              if(f0>447)
                                DIV = 7;              //SMCLK 为62.5K

                              else

                                if(f0>234)
                                  DIV = 8;          //SMCLK 为32768HZ
                                else
                                  if(f0>117)
                                    DIV = 9;       //SMCLK为16384HZ
                                else
                                  if(f0>58)
                                    DIV = 10;       //SMCLK为 8194HZ
                                else
                                  if(f0>29)
                                    DIV = 11;      //SMCLK 为4096HZ
                                else
                                  if(f0>14)
                                    DIV = 12;      //SMCLK 为2048HZ
                                else
                                  if(f0>7 )
                                    DIV = 13;        //SMCLK 为1024HZ

    switch(DIV)
    {
      case 1:
             if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(4000000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(4000000,f0,WorkMode));

             break;
      case 2:
               if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(2000000,f0,WorkMode));
               else
                MAX262_SetBF( MAX262_GetFN(2000000,f0,WorkMode));

             break;
      case 3:
               if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(1000000,f0,WorkMode));
               else
                MAX262_SetBF( MAX262_GetFN(1000000,f0,WorkMode));

             break;


      case 4:
              if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(500000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(500000,f0,WorkMode));

             break;



      case 5:
              if(Channel==channelA)
                MAX262_SetAF( MAX262_GetFN(250000,f0,WorkMode));
             else
                MAX262_SetBF( MAX262_GetFN(250000,f0,WorkMode));
              break;
      case 6:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(125000,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(125000,f0,WorkMode));
                   break;
      case 7:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(62500,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(62500,f0,WorkMode));
                   break;
      case 8:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(32768,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(32768,f0,WorkMode));
                   break;
      case 9:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(16384,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(16384,f0,WorkMode));
                   break;
      case 10:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(8192,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(8192,f0,WorkMode));
                   break;
      case 11:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(4096,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(4096,f0,WorkMode));
                   break;
      case 12:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(2048,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(2048,f0,WorkMode));
                   break;
      case 13:
                   if(Channel==channelA)
                     MAX262_SetAF( MAX262_GetFN(1024,f0,WorkMode));
                  else
                     MAX262_SetBF( MAX262_GetFN(1024,f0,WorkMode));
                   break;
      default: break;
    }
     SMCLK_Configure(DIV);
}


