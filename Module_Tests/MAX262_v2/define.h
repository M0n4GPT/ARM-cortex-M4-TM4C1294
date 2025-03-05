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
 * @brief       宏定义头文件，存放声明的常量
 *
 */

#ifndef DEFINE_H_
#define DEFINE_H_


//#################################################################

/************************************************************************
 * 枚举定义
 *************************************************************************/
/*********************各工作模式枚举***********************/
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
 * 宏定义
 *************************************************************************/
//#define TIVA_MAIN_FREQUENCY							80000000		//80MHz	tiva主频
//#define	ADCWHEEL_NUM								1024				// 滚轮转换值
//#define SHOW_MENU_NUM								6				// 菜单选项总量
//#################################################################

#endif /* DEFINE_H_ */
