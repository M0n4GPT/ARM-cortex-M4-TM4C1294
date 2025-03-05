/*
 * TIVA Cortex M4 MCU Experiment Application
 * Copyright (c) 2013-2014 China JiLiang University
 * All Rights Reserved.
 */

/*!
 * @file        Define.h
 * @version     1.0
 * @author      Kai Zhang
 * @date        2013-12-21
 * @brief       �궨��ͷ�ļ�����������ĳ���
 *
 */

#ifndef DEFINE_H_
#define DEFINE_H_


//#################################################################

/************************************************************************
 * ö�ٶ���
 *************************************************************************/
/*********************������ģʽö��***********************/
enum {
	 channelA=0,
	 channelB,

	 workMode
 };
enum  {
	 lowPass=0,
	 highPass,
	 bandPass,
	 allPass,

	 passMode
 };
enum {
	 modeOne=0,
	 modeTwo,
	 modeThree,
	 modeFour,

	 Channel
 };
//#################################################################
/************************************************************************
 * �궨��
 *************************************************************************/
//#define TIVA_MAIN_FREQUENCY							80000000		//80MHz	tiva��Ƶ
//#define	ADCWHEEL_NUM								1024				// ����ת��ֵ
//#define SHOW_MENU_NUM								6				// �˵�ѡ������
//#################################################################

#endif /* DEFINE_H_ */
