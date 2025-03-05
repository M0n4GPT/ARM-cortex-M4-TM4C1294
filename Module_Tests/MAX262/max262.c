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
�������ƣ�MAX262_Write()
�������ܣ���MAX262����дֵ��Ϊ����Ĳ��������ṩ���
��ڲ�������д���ַ����д��ֵ
���ڲ�������
����˵��������Pb����Ϊ�ӿڡ���λ����λ����ΪA3/A2/A1/A0/D1/D0/WR/��λ
          ֵÿ��ֻ��д��λ
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
�������ƣ�MAX262_SetAF()
�������ܣ���MAX262 AFNֵд��
��ڲ�������д��ֵ
���ڲ�������
����˵����Fֵ��6λ��������д��  ��ַ�ֱ�Ϊ1��2��3
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
�������ƣ�MAX262_SetBF()
�������ܣ���MAX262 BFNֵд��
��ڲ�������д��ֵ
���ڲ�������
����˵����Fֵ��6λ��������д�� ��ַ�ֱ�Ϊ9��10��11
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
�������ƣ�MAX262_SetAQ()
�������ܣ���MAX262 AQNֵд��
��ڲ�������д��ֵ
���ڲ�������
����˵����Qֵ��7λ�����Ĵδ�д�� ��ַ�ֱ�Ϊ 4��5��6��7
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
�������ƣ�MAX262_SetBQ()
�������ܣ���MAX262 BQNֵд��
��ڲ�������д��ֵ
���ڲ�������
����˵����Qֵ��7λ�����Ĵδ�д�� ��ַ�ֱ�Ϊ 12��13��14��15
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
�������ƣ�MAX262_LH_Getf0()
�������ܣ������ⲿ�����fc��Qֵȷ������Ƶ��f0
��ڲ�������ֹƵ��fc��Ʒ������Q���˲�ģʽ
���ڲ���������Ƶ��f0
����˵�����������ڵ�ͨ�͸�ͨ
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
�������ƣ�MAX262_GetFN
�������ܣ���������Ƶ��f0��max262ϵͳʱ�Ӽ��乤��ģʽȷ��FNֵ
��ڲ�����max262ϵͳʱ��fclk������Ƶ��f0�� ����ģʽWorkMode
���ڲ�����FNֵ
����˵����
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
�������ƣ�MAX262_LH
�������ܣ����õ�ͨ����ͨ��
��ڲ�������ֹƵ��fc��Ʒ������Q���˲�ģʽ������ģʽWorkMode��ͨ��
���ڲ�������
����˵���������ڵ�ͨ�͸�ͨ
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
	      WorkMode=modeThree;	 //��ֻͨ�й���ģʽ3
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
        DIV = 1;    //1��Ƶ smclk 4M fclk4M
      else
        if(f0>14306)
          DIV = 2;  //4��Ƶ smclk 2M fclk 2M
            else
              if(f0>7153)
                DIV = 3;  //8��Ƶ smclk 1M fclk 1M
                  else
                    if(f0>3576)
                      DIV = 4;   //16��Ƶ smclk 500K fclk 500k
                        else
                          if(f0>1788)
                            DIV = 5;  //32��Ƶ SMCLK 250K fclk 250K
                            else
                              if(f0>894)

                                DIV = 6;                //SMCLK Ϊ125K
                            else
                              if(f0>447)
                                DIV = 7;              //SMCLK Ϊ62.5K
                              else
                                if(f0>234)
                                  DIV = 8;          //SMCLK Ϊ32768HZ
                                else
                                  if(f0>117)
                                    DIV = 9;       //SMCLKΪ16384HZ
                                else
                                  if(f0>58)
                                    DIV = 10;       //SMCLKΪ 8194HZ
                                else
                                  if(f0>29)
                                    DIV = 11;      //SMCLK Ϊ4096HZ
                                else
                                  if(f0>14)
                                    DIV = 12;      //SMCLK Ϊ2048HZ
                                else
                                  if(f0>7 )
                                    DIV = 13;        //SMCLK ��1024HZ

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
�������ƣ�MAX262_BP
�������ܣ����ô�ͨ
��ڲ��������޽�ֹƵ��fh�����޽�ֹƵ��fl������ģʽWorkMode��ͨ��
���ڲ�������
����˵���������ڴ�ͨ,����ʱע���һ�����������޽�ֹƵ�ʣ��ڶ��������޽�ֹƵ��

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
        DIV = 1;    //2��Ƶ smclk 4M fclk4M
      else
        if(f0>14306)
          DIV = 2;  //4��Ƶ smclk 2M fclk 2M
            else
              if(f0>7153)
                DIV = 3;  //8��Ƶ smclk 1M fclk 1M
                  else
                    if(f0>3576)
                      DIV = 4;   //16��Ƶ smclk 500K fclk 500k
                        else
                          if(f0>1788)
                            DIV = 5;  //32��Ƶ smclk 250K fclk 250K
                            else
                              if(f0>894)
                               DIV = 6;                //SMCLK Ϊ125K
                            else
                              if(f0>447)
                                DIV = 7;              //SMCLK Ϊ62.5K

                              else

                                if(f0>234)
                                  DIV = 8;          //SMCLK Ϊ32768HZ
                                else
                                  if(f0>117)
                                    DIV = 9;       //SMCLKΪ16384HZ
                                else
                                  if(f0>58)
                                    DIV = 10;       //SMCLKΪ 8194HZ
                                else
                                  if(f0>29)
                                    DIV = 11;      //SMCLK Ϊ4096HZ
                                else
                                  if(f0>14)
                                    DIV = 12;      //SMCLK Ϊ2048HZ
                                else
                                  if(f0>7 )
                                    DIV = 13;        //SMCLK Ϊ1024HZ

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


