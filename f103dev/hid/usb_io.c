/**
  ******************************************************************************
  * @file    usbio.c
  * $Author: wdluo $
  * $Revision: 67 $
  * $Date:: 2012-08-15 19:00:29 +0800 #$
  * @brief   USB�ϲ㺯��.
  ******************************************************************************
  * @attention
  *
  *<h3><center>&copy; Copyright 2009-2012, ViewTool</center>
  *<center><a href="http:\\www.viewtool.com">http://www.viewtool.com</a></center>
  *<center>All Rights Reserved</center></h3>
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "usb_istr.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include "usb_pwr.h"

#include "usb_io.h"
#include "string.h"



#define USB_DISCONNECT                      GPIOC
#define USB_DISCONNECT_PIN                  GPIO_Pin_3
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOC


uint8_t USB_RecvData[512] = {0x00};                 //USB��������
uint16_t USB_RecvDataLen = 0;                       //�������ݳ���

uint8_t *USB_SendData = 0;                          //��������
uint16_t USB_SendDataLen = 0;                       //�������ݳ���




/* Private functions ---------------------------------------------------------*/
void USB_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    //���ʱ�ӳ�ʼ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);
    RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
    
    //USB�������ų�ʼ��
    GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //USB�豸��ʼ��
    USB_Init();
    
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
}

void USB_IO_Int(void)
{
    USB_Istr();
}

static uint16_t usbSendCount = 0;

void USB_IO_SendData(uint8_t *dat, uint16_t len)
{
    USB_SendData = dat;
    USB_SendDataLen = len;
    usbSendCount = 0;

    USB_IO_SendDataC();
}

void USB_IO_SendDataC()
{
    if(bDeviceState != CONFIGURED)return;
    if(USB_SendDataLen > USB_MAXDATASIZE)                           //���ڶ˵�һ���շ�����
    {
        USB_SendDataLen = USB_SendDataLen - USB_MAXDATASIZE;        //�����Լ�
        USB_SIL_Write(ENDP2, (USB_SendData + (usbSendCount * USB_MAXDATASIZE)), USB_MAXDATASIZE); //���Ͳ�������
        usbSendCount++;
        SetEPTxCount(ENDP2, USB_MAXDATASIZE);
        SetEPTxValid(ENDP2);
    }
    else
    {
        if(USB_SendDataLen > 0)
        {
            USB_SIL_Write(ENDP2, (USB_SendData + (usbSendCount * USB_MAXDATASIZE)), USB_SendDataLen);
            SetEPTxCount(ENDP2, USB_MAXDATASIZE);                   //��������ȫ��
            SetEPTxValid(ENDP2);
            USB_SendDataLen = 0;
            usbSendCount = 0;
        }
    }
}

void USB_IO_RecvDataC()
{
    uint16_t countOne;
    static uint16_t countAll = 0, countNow = 0;

    countOne = USB_SIL_Read(ENDP1, USB_RecvData + countNow);        //��ȡ���ν������ݼ�����
    SetEPRxValid(ENDP1);
    if(!countNow)                                                   //Э������������ݳ���
    {
        countAll = USB_RecvData[2];
        countAll = ((countAll << 8) & 0xff00) + USB_RecvData[1] + 10;
        if(countAll > sizeof(USB_RecvData))
        {
            countAll = 10;
        }
    }
    countNow = countOne + countNow;                                 //�����������ݳ���
    if(countOne == USB_MAXDATASIZE)
    {
        if(countNow >= countAll)                                    //������ȫ
        {
            USB_RecvDataLen = countNow;
            countNow = 0;
        }
    }
    else                                                            //���ղ����˵�һ���շ��ļ���
    {
        USB_RecvDataLen = countNow;
        countNow = 0;
    }
}



/*********************************END OF FILE**********************************/

