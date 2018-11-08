/**
  ******************************************************************************
  * @file    usbio.h
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   USB�ϲ㺯������.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USBIO_H_
#define _USBIO_H_

#include "stm32f10x.h"

#include  "usbd_customhid_core.h"


#define USB_MAXDATASIZE 0x40                                //һ������

/* Exported Functions --------------------------------------------------------*/
extern uint8_t USB_RecvData[512];                           //���յ�������
extern uint16_t USB_RecvDataLen;                            //���ݳ���
extern USB_OTG_CORE_HANDLE USB_OTG_dev;


void USB_IO_DeInit(void);                                   //USB����ʼ��
void USB_IO_Init(void);                                     //USB��ʼ��
void USB_IO_Int(void);                                      //USB�ж�

void USB_IO_SendData(uint8_t *dat, uint16_t len);           //��������
void USB_IO_SendInt(uint8_t hasCard);                       //�����ж�����

void USB_IO_SendDataC(void);                                //��������ʵ��
void USB_IO_RecvDataC(uint16_t rcvCount);                   //��������ʵ��

void USB_TIMxInt(void);                                     //���ݰ���ʱ�ж�


#endif //_USBIO_H_
