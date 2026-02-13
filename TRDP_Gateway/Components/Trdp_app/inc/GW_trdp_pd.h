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
 
#ifndef GW_TRDP_PD_H
#define GW_TRDP_PD_H
 
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
#define MAX_PD_COMID            15U
#define DATA_MAX                1432U
#define TRDP_FRAME_PERAMETER    214U
#define PD_PP_DATA_READY        1U
#define PD_PP_DATA_NOT_READY    0U
#define PD_PP_DATA_RECIVE       1U
#define PD_PP_DATA_NOT_RECIVE   0U
#define DEF_EER_STRING          "No data available"


#define PD_DATA_SET             1000U
#define PD_DATA_FRAME_MEMBER    16U


/***********************************************************************
* TYPES
***********************************************************************/

typedef enum 
{
    MSG_TYPE_PD_DATA          = TRDP_MSG_PD,          /* Pd Data        0x5064u */
    MSG_TYPE_PD_REQUEST_REPLY = TRDP_MSG_PP,          /* Pd Pull Reply  0x5070u */
    MSG_TYPE_PD_REQUEST       = TRDP_MSG_PR,          /* Pd Request     0x5072u */
    MSG_TYPE_PD_ERROR         = TRDP_MSG_PE           /* Pd Error       0x5065u */
} PACKED msgPdType_e;

typedef enum
{
    NO_ERROR                   = 0,
    RECIVED_FRAME_SIZE_IS_LESS = 1,
    RECIVED_BUFFER_IS_EMPTY    = 2
}PACKED msgPdErr_e;

/* Configuration for TRDP PD  message Type */
typedef struct trdpPdMsgConfig
{
    bool        bEnable;
    uint32_t    ui32SendComId;
    uint32_t    ui32TrdpPollIntervel;  /* time is in microseconds */
    uint32_t    ui32TrdpDestIp;
    uint32_t    ui32TrdpSrcIp1;
    uint32_t    ui32TrdpSrcIp2;
    TRDP_PUB_T  stPubHandle;
    TRDP_SUB_T  stSubHandle;
    uint8_t     ui8TrdpMsgBuff[DATA_MAX + TRDP_FRAME_PERAMETER];
    uint16_t    ui16TrdpMsgDataLen;
    uint8_t     ui8newData;
    msgPdType_e eMsgPdType;
} trdpPdMsgConfig_t;

typedef struct trdpPdMsg
{
    TRDP_PD_INFO_T stPdMsg;
    uint8_t        pui8TrdpMsg[DATA_MAX];
    uint16_t       ui16TrdpMsgLen;
}  PACKED trdpPdMsg_t;


/* X macrp for Trdp Pd configuration  */
/*   <CONFIGURATION No>  <          ENABLE OR DISABLE              >  <                   CONFIGURATION COM ID         >   <                    Trdp interval                     >   <              DESTINATION IP ADDRESS            >   <                SOURCE IP ADDRESS               >   <                Source IP ADDRESS              >   <        PUBLISH HANDEL                        >   <  SUBSCRIBER HANDEL                          >   <                TRDP MESSAGE BUFFER             >   <               TRDP MESSAGE LENGTH                  >   <            NEW DATA RECIVED                >   <                    TRDPMSG TYPE             >*/ 
#define TRDP_PD_MSG_CONFIG \
    X(TRDP_PD_CONFIG_1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_1].eMsgPdType ) \
    X(TRDP_PD_CONFIG_2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_2].eMsgPdType ) \
    X(TRDP_PD_CONFIG_3,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_3].eMsgPdType ) \
    X(TRDP_PD_CONFIG_4,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_4].eMsgPdType ) \
    X(TRDP_PD_CONFIG_5,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_5].eMsgPdType ) \
    X(TRDP_PD_CONFIG_6,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_6].eMsgPdType ) \
    X(TRDP_PD_CONFIG_7,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_7].eMsgPdType ) \
    X(TRDP_PD_CONFIG_8,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_8].eMsgPdType ) \
    X(TRDP_PD_CONFIG_9,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].bEnable,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui32SendComId,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui32TrdpPollIntervel,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui32TrdpDestIp,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui32TrdpSrcIp1,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui32TrdpSrcIp2,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].stPubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].stSubHandle,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui8TrdpMsgBuff,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui16TrdpMsgDataLen,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].ui8newData,  stTrdpPdMsgConfig[TRDP_PD_CONFIG_9].eMsgPdType ) \
    X(TRDP_PD_CONFIG_10, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_10].eMsgPdType ) \
    X(TRDP_PD_CONFIG_11, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_11].eMsgPdType ) \
    X(TRDP_PD_CONFIG_12, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_12].eMsgPdType ) \
    X(TRDP_PD_CONFIG_13, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_13].eMsgPdType ) \
    X(TRDP_PD_CONFIG_14, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_14].eMsgPdType ) \
    X(TRDP_PD_CONFIG_15, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].bEnable, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui32SendComId, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui32TrdpPollIntervel, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui32TrdpDestIp, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui32TrdpSrcIp1, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui32TrdpSrcIp2, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].stPubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].stSubHandle, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui8TrdpMsgBuff, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui16TrdpMsgDataLen, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].ui8newData, stTrdpPdMsgConfig[TRDP_PD_CONFIG_15].eMsgPdType ) \

enum
{
    #define X(TRDP_MSGID, ENABLE, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE ) TRDP_MSGID,
        TRDP_PD_MSG_CONFIG
    #undef X
};



/***********************************************************************
* PUBLIC VARIABLES 
***********************************************************************/
/* Configuration for the Pd Message type */
extern trdpPdMsgConfig_t stTrdpPdMsgConfig[MAX_PD_COMID];

extern TRDP_DATASET_T  gDataSetPd;

/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/
/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void* pvEthTrdp(void* arg);

/**
 * @brief 
 * 
 * @param pRefCon 
 * @param appHandle 
 * @param pMsg 
 * @param pData 
 * @param dataSize 
 */
void pdCallback(void *pRefCon, TRDP_APP_SESSION_T appHandle, const TRDP_PD_INFO_T *pMsg, 
                UINT8 *pData, UINT32 dataSize);

/**
 * @brief update the Trdp Queue for sending pd data and pd reply and also update the application message buffer 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eSendTrdpPdData(trdpPdMsg_t *stPdTrdpData, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle );

/**
 * @brief Init Trdp pd stack as per the configuration
 * 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eInitTrdpPdStack(void);

/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
