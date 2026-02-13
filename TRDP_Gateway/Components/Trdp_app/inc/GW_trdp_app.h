/*
 *      Copyright (c) 2025 Bacancy System Pvt Ltd.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *      @author: BACANCY SYSTEMS PVT. LTD. (Developer: Vivek pal) 
 *      Date:    JAN 2025
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
 
#ifndef GW_TRDP_APP_H
#define GW_TRDP_APP_H
 
/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include "trdp_if_light.h"
#include "vos_thread.h"
#include "vos_sock.h"
#include "vos_utils.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include "GW_common.h"
#include "tau_marshall.h"

/***********************************************************************
 * CONSTANTS
 **********************************************************************/
#define UART_FRAME_HEADER_LEN   7U
#define UART_END_PARAMETERS     3U
/***********************************************************************
* TYPES
***********************************************************************/

// typedef enum
// {
//     MD_NO_ERROR           = 0,
//     MD_TIMEOUT_ERROR      = 1,
//     MD_REPLY_TIMEOUT      = 2,
//     MD_CONFIRM_TIMEOUT    = 3
// } PACKED msgMdErr_e;

typedef enum 
{
    TRDP_PD      = 0,
    TRDP_MD      = 1,
    UNKNOWN_TYPE = 2
}PACKED trdpMsgType_e;

/***********************************************************************
* PUBLIC VARIABLES 
***********************************************************************/
 extern UINT32  ui32OwnIP;
 extern UINT32  *refCon;
 extern TRDP_APP_SESSION_T appHandle;

/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/
/**
 * @brief Convert IP address from decimal to string for reader useablity
 * 
 * @param ipAddress 
 * @param strIp 
 */
 void strGetIpInString(uint32_t ipAddress, char *strIp);

 /**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @return trdpMsgType_e 
 */
trdpMsgType_e eGetTrdpMsgType(uint8_t * pu8DataBuff);

/**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @param appHandle 
 * @return trdpMsgType_e 
 */
trdpMsgType_e eSendTrdpData(uint8_t * pu8DataBuff, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle);


/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
