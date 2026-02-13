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
 
#ifndef GW_TRDP_MD_H
#define GW_TRDP_MD_H
 
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

/***********************************************************************
 * CONSTANTS
 **********************************************************************/
/* for TRDP MD message  */
#define MAX_MD_COMID             5U
#define MD_DATA_MAX              65388U
#define TRDP_MD_FRAME_PERAMETER  311U

#define MD_DATA_SET              2000U
#define MD_DATA_FRAME_MEMBER     24U


/***********************************************************************
* TYPES
***********************************************************************/
typedef enum
{
    MSG_TYPE_MD_NOTIFICATION = TRDP_MSG_MN,  /* 0x4D6E */
    MSG_TYPE_MD_REQUEST      = TRDP_MSG_MR,  /* 0x4D72 */
    MSG_TYPE_MD_REPLY        = TRDP_MSG_MP,  /* 0x4D70 */
    MSG_TYPE_MD_REPLY_QUERY  = TRDP_MSG_MQ,  /* 0x4D71 */
    MSG_TYPE_MD_CONFIRM      = TRDP_MSG_MC,  /* 0x4D63 */
    MSG_TYPE_MD_ERROR        = TRDP_MSG_ME   /* 0x4D65 */
} PACKED msgMdType_e;

typedef enum
{
    NO_ERROR_MD                   = 0,
    MD_TIMEOUT_ERROR              = 1,
    MD_REPLY_TIMEOUT              = 2,
    MD_CONFIRM_TIMEOUT            = 3,
    RECIVED_MD_FRAME_SIZE_IS_LESS = 4,
    RECIVED_MD_BUFFER_IS_EMPTY    = 5
}PACKED msgMdErr_e;

typedef struct trdpMdMsg
{
    TRDP_MD_INFO_T stMdMsg;
    uint8_t        pui8TrdpMsg[MD_DATA_MAX];
    uint32_t       ui32TrdpMsgLen;
    TRDP_UUID_T    sessionId;
} PACKED trdpMdMsg_t;

typedef struct trdpMdMsgBuff
{
    uint8_t        pui8TrdpMsg[MD_DATA_MAX];
    uint32_t       ui32TrdpMsgLen;
} PACKED trdpMdMsgBuff_t;

/* Configuration for TRDP MD  message Type */
typedef struct trdpMdMsgConfig
{
    bool            bEnable;
    uint32_t        ui32ComId;
    uint32_t        ui32Timeout;             /* in microseconds */
    uint32_t        ui32TrdpMulticastIp;
    uint32_t        ui32TrdpSrcLowIp;
    uint32_t        ui32TrdpSrcHighIp;
    TRDP_LIS_T      stListenHandleUdp;       /* UDP listener */
    TRDP_LIS_T      stListenHandleTcp;       /* TCP listener */
    trdpMdMsgBuff_t stTrdpMsgBuff;
    trdpMdMsgBuff_t stTrdpRequestMsgBuff;
    uint8_t         ui8NewData;
    msgMdType_e     eMsgMdType;
    bool            bExpectReply;
    bool            bConfirmRequired;
    bool            bUseTcp;                 /* TRUE=TCP, FALSE=UDP */
} trdpMdMsgConfig_t;

/* X macrp for Trdp Md configuration  */
/*   <CONFIGURATION No>  <          ENABLE OR DISABLE              >  <              CONFIGURATION COM ID        >   <                    TimeOut                 >  <              DESTINATION IP ADDRESS            >   <                SOURCE IP LOWER LIMIT ADDRESS         >  <        SOURCE IP UPPER LIMIT ADDRESS           >     <                      HANDEL                      >   <                TRDP MESSAGE BUFFER             >     <                TRDP MESSAGE BUFFER             >    <            MSG Type                      >   <         CONFORMATION REQUIRED                   >*/
#define TRDP_MD_MSG_CONFIG \
    Y(TRDP_MD_CONFIG_1, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].bEnable, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].ui32ComId, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].ui32Timeout, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].ui32TrdpMulticastIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].ui32TrdpSrcLowIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].ui32TrdpSrcHighIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].stListenHandleUdp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].stTrdpMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].stTrdpRequestMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].eMsgMdType, stTrdpMdMsgConfig[TRDP_MD_CONFIG_1].bConfirmRequired )\
    Y(TRDP_MD_CONFIG_2, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].bEnable, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].ui32ComId, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].ui32Timeout, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].ui32TrdpMulticastIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].ui32TrdpSrcLowIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].ui32TrdpSrcHighIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].stListenHandleUdp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].stTrdpMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].stTrdpRequestMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].eMsgMdType, stTrdpMdMsgConfig[TRDP_MD_CONFIG_2].bConfirmRequired )\
    Y(TRDP_MD_CONFIG_3, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].bEnable, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].ui32ComId, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].ui32Timeout, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].ui32TrdpMulticastIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].ui32TrdpSrcLowIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].ui32TrdpSrcHighIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].stListenHandleUdp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].stTrdpMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].stTrdpRequestMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].eMsgMdType, stTrdpMdMsgConfig[TRDP_MD_CONFIG_3].bConfirmRequired )\
    Y(TRDP_MD_CONFIG_4, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].bEnable, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].ui32ComId, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].ui32Timeout, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].ui32TrdpMulticastIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].ui32TrdpSrcLowIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].ui32TrdpSrcHighIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].stListenHandleUdp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].stTrdpMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].stTrdpRequestMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].eMsgMdType, stTrdpMdMsgConfig[TRDP_MD_CONFIG_4].bConfirmRequired )\
    Y(TRDP_MD_CONFIG_5, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].bEnable, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].ui32ComId, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].ui32Timeout, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].ui32TrdpMulticastIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].ui32TrdpSrcLowIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].ui32TrdpSrcHighIp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].stListenHandleUdp, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].stTrdpMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].stTrdpRequestMsgBuff, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].eMsgMdType, stTrdpMdMsgConfig[TRDP_MD_CONFIG_5].bConfirmRequired )\

enum
{
    #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE ) TRDP_MD_MSG_ID,
        TRDP_MD_MSG_CONFIG
    #undef Y
};

/***********************************************************************
* PUBLIC VARIABLES 
***********************************************************************/
/* Configuration for the Md Message type */
extern trdpMdMsgConfig_t stTrdpMdMsgConfig[MAX_MD_COMID];

/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/
/**
 * @brief MD callback function for receiving MD messages
 * 
 * @param pRefCon User reference
 * @param appHandle Application handle
 * @param pMsg MD message info
 * @param pData Message data
 * @param u32DataSize Data size
 */
void mdCallback(void                   *pRefCon,
                TRDP_APP_SESSION_T     appHandle,
                const TRDP_MD_INFO_T   *pMsg,
                UINT8                  *pData,
                UINT32                 u32DataSize);

/**
 * @brief 
 * 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eInitTrdpMdStack(void);

/**
 * @brief Send MD message from UART data
 * 
 * @param pu8DataBuff Data buffer
 * @param u32DataSize Data size
 * @param msgType MD message type
 * @return TRDP_ERR_T Error code
 */
TRDP_ERR_T eSendMdMessage(uint8_t *pu8DataBuff, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle);

/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
