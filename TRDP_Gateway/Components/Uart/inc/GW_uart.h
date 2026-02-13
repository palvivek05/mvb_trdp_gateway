/*
 *      Copyright (c) 2024 Bacancy System Pvt Ltd.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *      @author: BACANCY SYSTEMS PVT. LTD. (Developer: VRUSHAL VARA) 
 *      Date:    OCT 2025
 *
 *
 *                      CONFIDENTIAL INFORMATION
 *                      ------------------------
 *      This Document contains Confidential Information or Trade Secrets,
 *      or both, which are the property of Bacancy System Pvt. Ltd.
 *      This document may not be copied, reproduced, reduced to any
 *      electronic medium or machine readable form or otherwise
 *      duplicated and the information herein may not be used,
 *      disseminated or otherwise disclosed, except with the prior
 *      written consent of Bacancy System Pvt. Ltd.
 *
 */
 /*!
 * \file
 * \brief
 */
 
#ifndef _GW_UART_H
#define _GW_UART_H
 
/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include "GW_common.h"
 
/***********************************************************************
 * CONSTANTS
 **********************************************************************/
#define EXTRA_BYTES_LEN         (0x0AU)
#define SOF_BYTE_VAL            (0x45U)
#define EOF_BYTE_VAL            (0x69U)
#define SOF_BYTE_POS            (0x00U)
#define LEN_LSB_BYTE_POS        (0x01U)
#define CMDID_LSB_BYTE_POS      (0x05U)
#define CMDID_MSB_BYTE_POS      (0x06U)
#define DATA_START_BYTE_POS     (0x07U)

#define MAX_UART_BUFF_SIZE      (66000U)

extern int iUartTtySTM2Type;
/***********************************************************************
* TYPES
***********************************************************************/
typedef struct UartRecBuff
{
    uint8_t     au8Data[MAX_UART_BUFF_SIZE];
    uint32_t    u32ReadBytesSize;
    uint8_t     u8DataReady;
}PACKED UartRecBuff_t;

/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/
int iUartInit(char *pcPort, int iBoardRate);
int iUartSend(int iUartType, uint16_t u16CommandId, uint8_t * pu8DataBuff, uint32_t u32DataSize);
int iUartRec(int iUartType, uint8_t * pu8DataBuff, uint32_t u32DataSize);
void *pvUartRxThread(void *arg);

/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
