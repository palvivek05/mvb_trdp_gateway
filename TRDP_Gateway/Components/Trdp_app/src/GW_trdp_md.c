/*
 *      Copyright (c) 2025 Bacancy System Pvt Ltd.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *      @author: BACANCY SYSTEMS PVT. LTD. (Developer: Vivek Pal) 
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

/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include "GW_trdp_md.h"
#include "GW_uart.h"
#include "GW_trdp_app.h"

/************************************************************************
 * Define Macros
************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
************************************************************************/

typedef enum
{
    TST_TCP         = 0x01,             /* execute TCP tests */
    TST_UCAST       = 0x02,             /* execute UDP unicast tests */
    TST_MCAST       = 0x04,             /* execute UDP multicast tests */
    TST_APROTO      = 0x07,             /* execute all protocols tests */
    TST_NOTIFY      = 0x10,             /* execute notification tests */
    TST_REQREP      = 0x20,             /* execute request/reply tests */
    TST_REQREPCFM   = 0x40,             /* execute request/reply/confirm tests */
    TST_APATTERN    = 0x70,             /* execute all patterns tests */
    TST_ALL         = 0x77,             /* execute all tests */
} Group;
/************************************************************************
 * Define Private Variables
************************************************************************/

/************************************************************************
 * Define Global Variables
************************************************************************/
/* Configuration for the Md Message type */
trdpMdMsgConfig_t stTrdpMdMsgConfig[MAX_MD_COMID];

TRDP_APP_SESSION_T appHandle1 ;

/**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
**********************************************************************/
/**
 * @brief 
 * 
 * @param comId 
 * @param destIp 
 * @param pData 
 * @param dataSize 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eSendMdRequest(UINT32 comId, TRDP_IP_ADDR_T destIp, UINT8 *pData, UINT32 dataSize);

/**
 * @brief 
 * 
 * @param comId 
 * @param destIp 
 * @param pData 
 * @param dataSize 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eSendMdNotify(UINT32 comId, TRDP_IP_ADDR_T destIp, UINT8 *pData, UINT32 dataSize);

/**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @return trdpMdMsg_t 
 */
static trdpMdMsg_t stTrdpMdDataFromUartFrame(uint8_t * pu8DataBuff, uint32_t u32DataSize);

/**
 * @brief 
 * 
 * @param stMdTrdpData 
 * @return msgMdErr_e 
 */
static msgMdErr_e eUpdateTrdpMdDataTable(trdpMdMsg_t * stMdTrdpData);

/**
 * @brief 
 * 
 * @param stMdTrdpData 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eUpdateTrdpMdStackQueue(trdpMdMsg_t * stMdTrdpData, TRDP_APP_SESSION_T appHandle);

/**
 * @brief 
 * 
 * @param info 
 */
static void vPrintTrdpMdInfo(const TRDP_MD_INFO_T *info);
/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/

/**
 * @brief Send MD request
 * 
 * @param comId COM ID
 * @param destIp Destination IP
 * @param pData Data to send
 * @param dataSize Data size
 * @return TRDP_ERR_T Error code
 */
static TRDP_ERR_T eSendMdRequest(UINT32 comId, TRDP_IP_ADDR_T destIp, UINT8 *pData, UINT32 dataSize)
{
    TRDP_ERR_T  err             = TRDP_NO_ERR;
    TRDP_UUID_T sessionId       = {0};
    UINT32      timeout         = 5000000u;  /* 5 seconds default */
    UINT32      expectedReplies = 1u;

    /* Find timeout in configuration */
    for (int i = 0; i < MAX_MD_COMID; i++)
    {
        if (stTrdpMdMsgConfig[i].bEnable && stTrdpMdMsgConfig[i].ui32ComId == comId)
        {
            timeout = stTrdpMdMsgConfig[i].ui32Timeout;
            break;
        }
    }

    vos_printLog(VOS_LOG_INFO, "-> Sending MD Request, COM ID: %u, timeout: %uus\n", comId, timeout);

    err = tlm_request(appHandle,
                     NULL,
                     mdCallback,
                     &sessionId,
                     comId,
                     0u,
                     0u,
                     ui32OwnIP,
                     destIp,
                     TRDP_FLAGS_CALLBACK,
                     expectedReplies,
                     timeout,
                     NULL,
                     pData,
                     dataSize,
                     NULL,
                     NULL);

    if (err != TRDP_NO_ERR)
    {
        vos_printLog(VOS_LOG_ERROR, "tlm_request failed, err: %d\n", err);
    }

    return err;
}

/**
 * @brief Send MD notification
 * 
 * @param comId COM ID
 * @param destIp Destination IP
 * @param pData Data to send
 * @param dataSize Data size
 * @return TRDP_ERR_T Error code
 */
static TRDP_ERR_T eSendMdNotify(uint32_t u32ComId, TRDP_IP_ADDR_T destIp, UINT8 *pData, UINT32 dataSize)
{
    TRDP_ERR_T err = TRDP_NO_ERR;
    vos_printLog(VOS_LOG_INFO, "-> Sending MD Notification, COM ID: %u\n", u32ComId);

    err = tlm_notify(appHandle,
                    NULL,
                    mdCallback,
                    u32ComId,
                    0u,
                    0u,
                    ui32OwnIP,
                    destIp,
                    TRDP_FLAGS_CALLBACK,
                    NULL,
                    pData,
                    dataSize,
                    NULL,
                    NULL);
    if (err != TRDP_NO_ERR)
    {
        vos_printLog(VOS_LOG_ERROR, "tlm_notify failed, err: %d\n", err);
    }

    return err;
}

/**
 * @brief Send MD message from UART data
 * 
 * @param pu8DataBuff Data buffer
 * @param u32DataSize Data size
 * @param msgType MD message type
 * @return TRDP_ERR_T Error code
 */
TRDP_ERR_T eSendMdMessage(uint8_t *pu8DataBuff, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle)
{
    TRDP_ERR_T  eErr   = TRDP_NO_ERR;
    msgMdErr_e  eMdErr = NO_ERROR_MD;

    trdpMdMsg_t stMdTrdpData = {0};

    if ((pu8DataBuff == NULL) || (u32DataSize == 0))
    {
        eErr = TRDP_PARAM_ERR;
    }
    else
    {
        stMdTrdpData = stTrdpMdDataFromUartFrame(pu8DataBuff,u32DataSize);

        if(stMdTrdpData.ui32TrdpMsgLen != 0) 
        {
            eMdErr = eUpdateTrdpMdDataTable(&stMdTrdpData);
            if (NO_ERROR_MD != eMdErr)
            {
                vos_printLog(VOS_LOG_ERROR, "failed to Update the table %d  \n", eMdErr );
            }
            else
            {
                eErr = eUpdateTrdpMdStackQueue(&stMdTrdpData, appHandle);
                if (eErr != TRDP_NO_ERR)
                {
                    vos_printLog(VOS_LOG_ERROR, "Unable to update trdp Md Stack %d \n", eErr );
                }
            }
        }
    }
    return eErr;
}

/**
 * @brief 
 * 
 * @param stMdTrdpData 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eUpdateTrdpMdStackQueue(trdpMdMsg_t * stMdTrdpData, TRDP_APP_SESSION_T appHandle)
{
    TRDP_ERR_T eErr = TRDP_NO_ERR;
    TRDP_UUID_T sessionId = {0};

    if (NULL != stMdTrdpData)
    {
        #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
        if((stMdTrdpData->stMdMsg.comId == COMID) && (TRUE == ENABLE))\
        {\
            switch (stMdTrdpData->stMdMsg.msgType)\
            {\
                case MSG_TYPE_MD_NOTIFICATION :\
                {\
                    vos_printLog(VOS_LOG_INFO, "-> Sending MD Notification, COM ID: %u\n", COMID);\
                    eErr = tlm_notify(appHandle,\
                    NULL,\
                    mdCallback,\
                    COMID,\
                    0u,\
                    0u,\
                    ui32OwnIP,\
                    stMdTrdpData->stMdMsg.destIpAddr,\
                    TRDP_FLAGS_CALLBACK,\
                    NULL,\
                    MSG_BUFF.pui8TrdpMsg,\
                    MSG_BUFF.ui32TrdpMsgLen,\
                    NULL,\
                    NULL);\
                    if (eErr != TRDP_NO_ERR)\
                    {\
                        vos_printLog(VOS_LOG_ERROR, "tlm_notify failed, err: %d\n", eErr);\
                    }\
                    else\
                    {\
                        vos_printLog(VOS_LOG_INFO, "sucsefully send the Md noticifation %d\n", eErr);\
                    }\
                }\
                break;\
                case MSG_TYPE_MD_REQUEST :\
                {\
                    eErr = tlm_request(appHandle,\
                    NULL,\
                    mdCallback,\
                    &sessionId,\
                    COMID,\
                    0u,\
                    0u,\
                    ui32OwnIP,\
                    stMdTrdpData->stMdMsg.destIpAddr,\
                    TRDP_FLAGS_CALLBACK,\
                    1, /* hard coded only 1 reply is expected  */\
                    TIMEOUT,\
                    NULL,\
                    MSG_REQ_BUFF.pui8TrdpMsg,\
                    MSG_REQ_BUFF.ui32TrdpMsgLen,\
                    NULL,\
                    NULL);\
                    if (eErr != TRDP_NO_ERR)\
                    {\
                        vos_printLog(VOS_LOG_ERROR, "tlm_request failed, err: %d\n", eErr);\
                    }\
                    else\
                    {\
                        vos_printLog(VOS_LOG_INFO, "Md Request has been sucsefully send  %d\n", eErr);\
                    }\
                }\
                break;\
                case MSG_TYPE_MD_REPLY :\
                {\
                    \
                }\
                break;\
                case MSG_TYPE_MD_REPLY_QUERY :\
                {\
                    \
                }\
                break;\
                case MSG_TYPE_MD_CONFIRM :\
                {\
                    \
                }\
                break;\
                case MSG_TYPE_MD_ERROR :\
                {\
                    /*TBD*/\
                }\
                break;\
                default:\
                {\
                    /*TBD*/\
                }\
                break;\
            }\
        }
            TRDP_MD_MSG_CONFIG
        #undef Y
    }
    else
    {
        eErr = TRDP_UNKNOWN_ERR;
    }

    return eErr;

}

/**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @return trdpMdMsg_t 
 */
static trdpMdMsg_t stTrdpMdDataFromUartFrame(uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    trdpMdMsg_t stMdTrdpData    = {0};
    TRDP_ERR_T  eErr            = TRDP_NO_ERR ;
    const uint32_t ui32SizeBuff = TRDP_MD_FRAME_PERAMETER ;

    if ((pu8DataBuff != NULL) || (0 == u32DataSize))
    {
        eErr = tau_unmarshall(refCon, MD_DATA_SET, pu8DataBuff + UART_FRAME_HEADER_LEN, (uint32_t) sizeof(stMdTrdpData.stMdMsg), (UINT8 *) &stMdTrdpData.stMdMsg, &ui32SizeBuff, NULL);
        vos_printLog(VOS_LOG_INFO, "***********Trdp Md data from Uart ***********\n");
        vPrintTrdpMdInfo(&stMdTrdpData.stMdMsg);
        vos_printLog(VOS_LOG_INFO, " ***********Trdp Md data from uart End ***********\n");
        stMdTrdpData.ui32TrdpMsgLen = u32DataSize - (sizeof(TRDP_MD_INFO_T) + UART_END_PARAMETERS);
        memcpy(stMdTrdpData.pui8TrdpMsg, (pu8DataBuff + UART_FRAME_HEADER_LEN + sizeof(TRDP_MD_INFO_T)), stMdTrdpData.ui32TrdpMsgLen );
    }
    else
    {
        memset(&stMdTrdpData,0,sizeof(stMdTrdpData));
    }

    return stMdTrdpData;
}

/**
 * @brief 
 * 
 * @param stMdTrdpData 
 * @return msgMdErr_e 
 */
static msgMdErr_e eUpdateTrdpMdDataTable(trdpMdMsg_t * stMdTrdpData)
{
    char       strDestIp[INET_ADDRSTRLEN]  = {0};
    msgMdErr_e eErr                        = NO_ERROR_MD;

    if(stMdTrdpData != NULL)
    {
        if ( 0 != stMdTrdpData->ui32TrdpMsgLen)
        {
            strGetIpInString(stMdTrdpData->stMdMsg.destIpAddr, strDestIp);
            vos_printLog(VOS_LOG_INFO, "Updating the Data table for COM ID %u with Dest IP %s \n",stMdTrdpData->stMdMsg.comId, strDestIp );

            /* Send each COMID respecting its interval */
            #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
                if((stMdTrdpData->stMdMsg.comId == COMID))\
                {\
                    MSG_BUFF.ui32TrdpMsgLen = stMdTrdpData->ui32TrdpMsgLen;\
                    memcpy(MSG_BUFF.pui8TrdpMsg, &stMdTrdpData->pui8TrdpMsg, MSG_BUFF.ui32TrdpMsgLen);\
                }\

            TRDP_MD_MSG_CONFIG
            #undef Y
        }
        else
        {
            eErr = RECIVED_MD_BUFFER_IS_EMPTY;
        }
    }
    else
    {
        eErr = RECIVED_MD_FRAME_SIZE_IS_LESS;

    }

    return eErr;

}

/**
 * @brief 
 * 
 * @param info 
 */
static void vPrintTrdpMdInfo(const TRDP_MD_INFO_T *stInfo)
{
    if (stInfo == NULL)
    {
        return;
    }

    vos_printLog(VOS_LOG_INFO, "---- TRDP_MD_INFO_T ----\n");
    vos_printLog(VOS_LOG_INFO, "srcIpAddr          : 0x%08X\n", stInfo->srcIpAddr);
    vos_printLog(VOS_LOG_INFO, "destIpAddr         : 0x%08X\n", stInfo->destIpAddr);
    vos_printLog(VOS_LOG_INFO, "seqCount           : %u\n",     stInfo->seqCount);
    vos_printLog(VOS_LOG_INFO, "protVersion        : %u\n",     stInfo->protVersion);
    vos_printLog(VOS_LOG_INFO, "msgType            : %X\n",     stInfo->msgType);
    vos_printLog(VOS_LOG_INFO, "comId              : %u\n",     stInfo->comId);
    vos_printLog(VOS_LOG_INFO, "etbTopoCnt         : %u\n",     stInfo->etbTopoCnt);
    vos_printLog(VOS_LOG_INFO, "opTrnTopoCnt       : %u\n",     stInfo->opTrnTopoCnt);
    vos_printLog(VOS_LOG_INFO, "aboutToDie         : %u\n",     stInfo->aboutToDie);
    vos_printLog(VOS_LOG_INFO, "numRepliesQuery    : %u\n",     stInfo->numRepliesQuery);
    vos_printLog(VOS_LOG_INFO, "numConfirmSent     : %u\n",     stInfo->numConfirmSent);
    vos_printLog(VOS_LOG_INFO, "numConfirmTimeout  : %u\n",     stInfo->numConfirmTimeout);
    vos_printLog(VOS_LOG_INFO, "userStatus         : %u\n",     stInfo->userStatus);
    vos_printLog(VOS_LOG_INFO, "replyStatus        : %u\n",     stInfo->replyStatus);
    vos_printLog(VOS_LOG_INFO, "sessionId          : %u\n",     stInfo->sessionId);
    vos_printLog(VOS_LOG_INFO, "replyTimeout (us)  : %u\n",     stInfo->replyTimeout);
    vos_printLog(VOS_LOG_INFO, "srcUserURI         : %u\n",     stInfo->srcUserURI);
    vos_printLog(VOS_LOG_INFO, "srcHostURI         : %u\n",     stInfo->srcHostURI);
    vos_printLog(VOS_LOG_INFO, "destUserURI        : %u\n",     stInfo->destUserURI);
    vos_printLog(VOS_LOG_INFO, "destHostURI        : %u\n",     stInfo->destHostURI);
    vos_printLog(VOS_LOG_INFO, "numExpReplies      : %u\n",     stInfo->numExpReplies);
    vos_printLog(VOS_LOG_INFO, "numReplies         : %u\n",     stInfo->numReplies);
    vos_printLog(VOS_LOG_INFO, "pUserRef           : %p\n",     stInfo->pUserRef);
    vos_printLog(VOS_LOG_INFO, "resultCode         : %u\n",     stInfo->resultCode);
    vos_printLog(VOS_LOG_INFO, "------------------------\n");
}


/***********************************************************************
 * PUBLIC FUNCTIONS
 **********************************************************************/

/**
 * @brief Initialize MD communication
 * 
 * @return TRDP_ERR_T Error code
 */
TRDP_ERR_T eInitTrdpMdStack(void)
{
    TRDP_ERR_T err       = TRDP_NO_ERR;
    TRDP_LIS_T mdHandler = NULL;

    #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_REQ_BUFF, MSG_LEN, MSG_TYPE, CONFIRM_REQUIRE )\
            /* Add UDP listener */\
            if (ENABLE == TRUE )\
            {\
                err = tlm_addListener(appHandle,\
                                     &mdHandler,\
                                     (void *) TST_UCAST,\
                                     NULL,\
                                     TRUE,\
                                     COMID,\
                                     0u,\
                                     0u,\
                                     SRC_LOW_IP,\
                                     SRC_HIGH_IP,\
                                     DEST_IP,\
                                     TRDP_FLAGS_CALLBACK,\
                                     NULL,\
                                     NULL);\
                if (err != TRDP_NO_ERR)\
                {\
                    vos_printLog(VOS_LOG_ERROR, "Failed to add MD listener for COM ID %u (UDP), err: %d \n",\
                                COMID, err);\
                    return err;\
                }\
            }

        TRDP_MD_MSG_CONFIG
    #undef Y

    vos_printLog(VOS_LOG_INFO, "succefully trdp md is initiated \n");

    return err;
}

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
                UINT32                 u32DataSize)
{
    char          strSrcIp[INET_ADDRSTRLEN]                               = {0};
    char          strDestIp[INET_ADDRSTRLEN]                              = {0};
    TRDP_ERR_T    eErr                                                    = TRDP_NO_ERR;
    const UINT32  bufSize                                                 = TRDP_MD_FRAME_PERAMETER ;
    UINT8         u8TrdpUartFrame[TRDP_MD_FRAME_PERAMETER + MD_DATA_MAX ] = "\0";

    strGetIpInString(pMsg->srcIpAddr, strSrcIp);
    strGetIpInString(pMsg->destIpAddr, strDestIp);

    if (0 != u32DataSize)
    {
        vos_printLog(VOS_LOG_INFO, "***********Trdp Md data from Network ***********\n");
        vPrintTrdpMdInfo(pMsg);
        vos_printLog(VOS_LOG_INFO, " ***********Trdp Md data from Network End ***********\n");
    }

    switch (pMsg->resultCode)
    {
        case TRDP_NO_ERR:
        {
            switch (pMsg->msgType)
            {
                case TRDP_MSG_MN:  /* MD Notification */
                {
                    vos_printLog(VOS_LOG_INFO, "<- MD Notification received, COM ID: %u, from: %s\n",pMsg->comId, strSrcIp);

                    #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
                        if ((pMsg->comId == COMID) && (u32DataSize > 0))\
                        {\
                            MSG_BUFF.ui32TrdpMsgLen = (u32DataSize < MD_DATA_MAX) ? u32DataSize : MD_DATA_MAX;\
                            /* vos_printLog(VOS_LOG_INFO, "   Data[%uB]: %.80s...\n", u32DataSize, pData); */\
                            /* Copy the recived data in Config message buffer */\
                            memcpy(MSG_BUFF.pui8TrdpMsg, pData, MSG_BUFF.ui32TrdpMsgLen );\
\
                            /* Add the Trdp Structure to coustom Trdp uart data */\
                            eErr = tau_marshall(refCon, MD_DATA_SET, (UINT8 *) pMsg, sizeof(*pMsg), u8TrdpUartFrame, &bufSize, NULL);\
                            if (eErr != TRDP_NO_ERR )\
                            {\
                                vos_printLog(VOS_LOG_ERROR, "Please check the uart frame formate, it must have Diffrent formate %d \n",eErr);\
                            }\
\
                            /* Appending the trdp data to coustom trdp uart frame */\
                            memcpy(&u8TrdpUartFrame[bufSize], MSG_BUFF.pui8TrdpMsg, MSG_BUFF.ui32TrdpMsgLen);\
\
                            /* sending data to to uart */\
                            iUartSend(iUartTtySTM2Type, (uint16_t)TRDP_MD, u8TrdpUartFrame, bufSize + MSG_BUFF.ui32TrdpMsgLen );\
                            vos_printLog(VOS_LOG_INFO, "the size of frame is %d and sie of data is %d  \n", MSG_BUFF.ui32TrdpMsgLen ,bufSize);\
                        }

                        TRDP_MD_MSG_CONFIG
                    #undef Y
                }
                break;
                case TRDP_MSG_MR:  /* MD Request */
                {
                    vos_printLog(VOS_LOG_INFO, "<- MD Request received, COM ID: %u, from: %s\n",pMsg->comId, strSrcIp);
                    if (pData != NULL && u32DataSize > 0)
                    {
                        vos_printLog(VOS_LOG_INFO, "   Data[%uB]: %.80s...\n", u32DataSize, pData);
                    }
                    /* Send reply based on configuration */
                    #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
                        if ((ENABLE == TRUE) && (COMID == pMsg->comId))\
                        {\
                            /* get the recived meaasge length  */\
                            MSG_REQ_BUFF.ui32TrdpMsgLen = (u32DataSize < MD_DATA_MAX) ? u32DataSize : MD_DATA_MAX;\
\
                            /*copy the Requested trdp Pd message date to buffer */\
                            memcpy(MSG_REQ_BUFF.pui8TrdpMsg, pData, MSG_REQ_BUFF.ui32TrdpMsgLen );\
\
                            /* Add the Trdp Structure to coustom Trdp uart data */\
                            eErr = tau_marshall(refCon, MD_DATA_SET, (UINT8 *) pMsg, sizeof(*pMsg), u8TrdpUartFrame, &bufSize, NULL);\
                            if(eErr != TRDP_NO_ERR)\
                            {\
                                vos_printLog(VOS_LOG_ERROR, "Please check the uart frame formate, it must have Diffrent formate %d \n",eErr);\
                            }\
\
                            /* Appending the trdp data to coustom trdp uart frame */\
                            memcpy(&u8TrdpUartFrame[bufSize], MSG_BUFF.pui8TrdpMsg, MSG_BUFF.ui32TrdpMsgLen);\
\
                            /* sending data on uart */\
                            iUartSend(iUartTtySTM2Type, (uint16_t)TRDP_MD, u8TrdpUartFrame, bufSize + MSG_BUFF.ui32TrdpMsgLen );\
\
                            if (TRUE == CONFIRM_REQUIRE)\
                            {\
                                vos_printLogStr(VOS_LOG_INFO, "-> Sending reply with confirmation request\n");\
                                eErr = tlm_replyQuery(appHandle,\
                                                    &pMsg->sessionId,\
                                                    pMsg->comId,\
                                                    0u,\
                                                    10000000u,\
                                                    NULL,\
                                                    MSG_REQ_BUFF.pui8TrdpMsg,\
                                                    MSG_REQ_BUFF.ui32TrdpMsgLen,\
                                                    NULL);\
                            }\
                            else\
                            {\
                                vos_printLogStr(VOS_LOG_INFO, "-> Sending reply\n");\
                                eErr = tlm_reply(appHandle,\
                                              &pMsg->sessionId,\
                                              pMsg->comId,\
                                              0,\
                                              NULL,\
                                              MSG_REQ_BUFF.pui8TrdpMsg,\
                                              MSG_REQ_BUFF.ui32TrdpMsgLen,\
                                              NULL);\
                            }\
                            if (eErr != TRDP_NO_ERR)\
                            {\
                                vos_printLog(VOS_LOG_ERROR, "Failed to send MD reply, err: %d\n", eErr);\
                            }\
                        }\

                        TRDP_MD_MSG_CONFIG
                    #undef Y
                }
                break;
                case TRDP_MSG_MP:  /* MD Reply */
                {
                    vos_printLog(VOS_LOG_INFO, "<- MD Reply received, COM ID: %u, from: %s\n", 
                                 pMsg->comId, strSrcIp);

                    if (pData != NULL && u32DataSize > 0)
                    {
                        vos_printLog(VOS_LOG_INFO, "   Data[%uB]: %.80s...\n", u32DataSize, pData);

                        /* Forward to UART */
                        UINT8 tempBuff[MD_DATA_MAX + TRDP_MD_FRAME_PERAMETER];
                        memcpy(tempBuff, pMsg, TRDP_MD_FRAME_PERAMETER);
                        memcpy(&tempBuff[TRDP_MD_FRAME_PERAMETER], pData, u32DataSize);
                        iUartSend(iUartTtySTM2Type,(uint16_t)TRDP_MD, tempBuff, u32DataSize + TRDP_MD_FRAME_PERAMETER);
                    }
                }
                break;
                case TRDP_MSG_MQ:  /* MD Reply with confirmation */
                {
                    vos_printLog(VOS_LOG_INFO, "<- MD Reply with confirmation received, COM ID: %u, from: %s\n",pMsg->comId, strSrcIp);
                    if (pData != NULL && u32DataSize > 0)
                    {
                        vos_printLog(VOS_LOG_INFO, "   Data[%uB]: %.80s...\n", u32DataSize, pData);
                    }
                    vos_printLogStr(VOS_LOG_INFO, "-> Sending confirmation\n");
                    eErr = tlm_confirm(appHandle, &pMsg->sessionId, 0, NULL);
                    if (eErr != TRDP_NO_ERR)
                    {
                        vos_printLog(VOS_LOG_ERROR, "Failed to send confirmation, err: %d\n", eErr);
                    }
                }
                break;
                case TRDP_MSG_MC:  /* MD Confirmation */
                {
                    vos_printLog(VOS_LOG_INFO, "<- MD Confirmation received, COM ID: %u\n", pMsg->comId);
                    /* Add the Trdp Structure to coustom Trdp uart data */\
                    eErr = tau_marshall(refCon, MD_DATA_SET, (UINT8 *) pMsg, sizeof(*pMsg), u8TrdpUartFrame, &bufSize, NULL);
                    iUartSend(iUartTtySTM2Type,(uint16_t)TRDP_MD, u8TrdpUartFrame, bufSize);
                }
                break;
                case TRDP_MSG_ME:  /* MD Error */
                {
                    vos_printLog(VOS_LOG_ERROR, "<- MD Error received, COM ID: %u\n", pMsg->comId);
                }
                break;
                default:
                {
                    /* TBD */
                }
                break;
            }
            break;
        }
        case TRDP_TIMEOUT_ERR:
        {
            vos_printLog(VOS_LOG_ERROR, "### MD Timeout (COM ID: %d, SrcIP: %s)\n",pMsg->comId, strSrcIp);
        }
        break;
        case TRDP_REPLYTO_ERR:
        {
            vos_printLog(VOS_LOG_ERROR, "### No MD Reply within timeout (COM ID: %d, DestIP: %s)\n",pMsg->comId, strDestIp);
        }
        break;
        case TRDP_CONFIRMTO_ERR:
        {
            /* TBD */
        }
        break;
        case TRDP_REQCONFIRMTO_ERR:
        {
            vos_printLog(VOS_LOG_ERROR, "### No MD Confirmation within timeout (COM ID: %d, DestIP: %s)\n",pMsg->comId, strDestIp);
        }
        break;
        default:
        {
            vos_printLog(VOS_LOG_ERROR, "### MD Error (COM ID: %d), err: %d\n",pMsg->comId, pMsg->resultCode);
        }
        break;
    }
}

/***********************************************************************
 * END OF FILE
 **********************************************************************/