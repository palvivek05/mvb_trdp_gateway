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
#include "GW_trdp_pd.h"
#include "GW_trdp_app.h"
#include "GW_uart.h"
#include "Udp_Rx.h"
#include "Udp_Rx.h"

/************************************************************************
 * Define Macros
************************************************************************/

/************************************************************************
 * Define Enumeration/Structure/Unions
************************************************************************/

/************************************************************************
 * Define Private Variables
************************************************************************/

/************************************************************************
 * Define Global Variables
************************************************************************/

/* Configuration for the Pd Message type */
trdpPdMsgConfig_t stTrdpPdMsgConfig[129];

/**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
**********************************************************************/
/**
 * @brief For updating the data table of trdp configuration message buffer according to recived data from uart and trdp
 * 
 * @param stPdTrdpData 
 * @return msgPdErr_e 
 */
static msgPdErr_e eUpdateTrdpPdDataTable(trdpPdMsg_t * stPdTrdpData);

 /**
  * @brief parse Trdp frame from uart recived Data
  * 
  * @param pu8DataBuff 
  * @param u32DataSize 
  * @return trdpPdMsg_t 
  */
 static trdpPdMsg_t stTrdpPdDataFromUartFrame(uint8_t * pu8DataBuff, uint32_t u32DataSize);

 /**
 * @brief updatde the trdp intrnal stack msg buffer 
 * 
 * @param stPdTrdpData 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eUpdateTrdpStackQueue(trdpPdMsg_t * stPdTrdpData, TRDP_APP_SESSION_T appHandle);

/**
 * @brief 
 * 
 * @param info 
 */
static void vPrintTrdpPdInfo(const TRDP_PD_INFO_T *info);

/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/
/**
 * @brief For updating the data table of trdp configuration message buffer according to recived data from uart and trdp
 * 
 * @param stPdTrdpData
 * @return msgPdErr_e
 */
static msgPdErr_e eUpdateTrdpPdDataTable(trdpPdMsg_t * stPdTrdpData)
{
    char       strDestIp[INET_ADDRSTRLEN]  = {0};
    msgPdErr_e eErr                        = NO_ERROR;

    if(stPdTrdpData != NULL)
    {
        if ( 0 != stPdTrdpData->ui16TrdpMsgLen )
        {
            strGetIpInString(stPdTrdpData->stPdMsg.destIpAddr, strDestIp);
            vos_printLog(VOS_LOG_INFO, "Updating the Data table for COM ID %u with Dest IP %s \n",stPdTrdpData->stPdMsg.comId, strDestIp );

            /* Send each COMID respecting its interval */
            #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
                if((stPdTrdpData->stPdMsg.comId == COMID))\
                {\
                    MSG_LEN = stPdTrdpData->ui16TrdpMsgLen;\
                    memcpy(MSG_BUFF,&stPdTrdpData->pui8TrdpMsg, MSG_LEN);\
                }\

            TRDP_PD_MSG_CONFIG
            #undef X
        }
        else
        {
            eErr = RECIVED_BUFFER_IS_EMPTY;
        }
    }
    else
    {
        eErr = RECIVED_FRAME_SIZE_IS_LESS;
    }

    return eErr;

}

/**
 * @brief updatde the trdp intrnal stack msg buffer 
 * 
 * @param stPdTrdpData 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eUpdateTrdpStackQueue(trdpPdMsg_t *stPdTrdpData, TRDP_APP_SESSION_T appHandle)
{
    TRDP_ERR_T eErr = TRDP_NO_ERR;

    if (NULL != stPdTrdpData)
    {
        #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
        if ((stPdTrdpData->stPdMsg.comId == COMID) && (ENABLE == TRUE)) \
        {\
            /*acquisition of the data in internal data base */\
            MSG_LEN = stPdTrdpData->ui16TrdpMsgLen;\
            memcpy(MSG_BUFF,&stPdTrdpData->pui8TrdpMsg, MSG_LEN);\
            switch(stPdTrdpData->stPdMsg.msgType)\
            {\
                case MSG_TYPE_PD_DATA :\
                {\
                    eErr = tlp_put(appHandle, HANDLER_PUB, MSG_BUFF, MSG_LEN);\
                    if (TRDP_NO_ERR != eErr)\
                    {\
                        vos_printLog(VOS_LOG_ERROR, "MSG_TYPE_PD_DATA unable to update for PD queue %d \n", eErr);\
                    }\
                    else\
                    {\
                        vos_printLog(VOS_LOG_INFO, "MSG_TYPE_PD_DATA Pd data has been recived from uart %d \n", eErr);\
                    }\
                }\
                break;\
                case MSG_TYPE_PD_REQUEST_REPLY :\
                {\
                    eErr = tlp_put(appHandle, HANDLER_PUB, MSG_BUFF, MSG_LEN);\
                    if (TRDP_NO_ERR != eErr)\
                    {\
                        vos_printLog(VOS_LOG_ERROR, "MSG_TYPE_PD_REQUEST_REPLY unable to update for PD  Reply queue %d \n", eErr);\
                    }\
                    else\
                    {\
                        vos_printLog(VOS_LOG_INFO, "MSG_TYPE_PD_REQUEST_REPLY The request Replay has been recived from uart for Com Id %d\n",stPdTrdpData->stPdMsg.comId);\
                    }\
                }\
                break;\
                case MSG_TYPE_PD_REQUEST :\
                {\
                    eErr = tlp_request(appHandle, HANDLER_SUB, 0, COMID, stPdTrdpData->stPdMsg.etbTopoCnt, stPdTrdpData->stPdMsg.opTrnTopoCnt, SRCIP_1, DEST_IP, 0, TRDP_FLAGS_NONE, 0u, MSG_BUFF, MSG_LEN, stPdTrdpData->stPdMsg.comId, stPdTrdpData->stPdMsg.replyIpAddr);\
                    if (TRDP_NO_ERR != eErr)\
                    {\
                        vos_printLog(VOS_LOG_ERROR, " MSG_TYPE_PD_REQUEST unable to update for PD Request queue %d \n", eErr);\
                    }\
                    else\
                    {\
                        vos_printLog(VOS_LOG_INFO, " MSG_TYPE_PD_REQUEST The request Replay has been recived from uart for Com Id %d \n",stPdTrdpData->stPdMsg.comId);\
                    }\
                }\
                break;\
                case MSG_TYPE_PD_ERROR :\
                {\
                   /* TBD */ \
                }\
                break;\
                default :\
                {\
                    vos_printLog(VOS_LOG_INFO, "Not fount In Configuration! \n");\
                }\
                break;\
            }\
        }

        TRDP_PD_MSG_CONFIG
        #undef X
    }
    else
    {
        eErr = TRDP_UNKNOWN_ERR;
    }

    return eErr;
}

 /**
  * @brief parse Trdp frame from uart recived Data
  * 
  * @param pu8DataBuff 
  * @param u32DataSize 
  * @return trdpPdMsg_t 
  */
static trdpPdMsg_t stTrdpPdDataFromUartFrame(uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    trdpPdMsg_t stPdTrdpData    = {0};
    TRDP_ERR_T eErr             = TRDP_NO_ERR ;
    const uint32_t ui32SizeBuff = TRDP_FRAME_PERAMETER;

    if (pu8DataBuff != NULL )
    {
        eErr = tau_unmarshall(refCon, PD_DATA_SET, (pu8DataBuff + UART_FRAME_HEADER_LEN), (uint32_t) sizeof(stPdTrdpData.stPdMsg), (UINT8 *) &stPdTrdpData.stPdMsg, &ui32SizeBuff, NULL);
        vos_printLog(VOS_LOG_INFO, "*************Trdp Pd data from Uart*************\n");
        vPrintTrdpPdInfo(&stPdTrdpData.stPdMsg);
        vos_printLog(VOS_LOG_INFO, "*************Trdp Pd data from Uart End*************\n");
        stPdTrdpData.ui16TrdpMsgLen = u32DataSize - (sizeof(TRDP_PD_INFO_T) + UART_END_PARAMETERS);
        memcpy(stPdTrdpData.pui8TrdpMsg, (pu8DataBuff + UART_FRAME_HEADER_LEN + TRDP_FRAME_PERAMETER), stPdTrdpData.ui16TrdpMsgLen );
    }
    else
    {
        memset(&stPdTrdpData,0,sizeof(stPdTrdpData));
    }

    return stPdTrdpData;
}

/**
 * @brief 
 * 
 * @param info 
 */
static void vPrintTrdpPdInfo(const TRDP_PD_INFO_T *stInfo)
{
    if (stInfo == NULL)
    {
        return;
    }
    vos_printLog(VOS_LOG_INFO,"---- TRDP_PD_INFO_T ----\n");
    vos_printLog(VOS_LOG_INFO,"srcIpAddr      : 0x%08X\n", stInfo->srcIpAddr);
    vos_printLog(VOS_LOG_INFO,"destIpAddr     : 0x%08X\n", stInfo->destIpAddr);
    vos_printLog(VOS_LOG_INFO,"seqCount       : %u\n",     stInfo->seqCount);
    vos_printLog(VOS_LOG_INFO,"protVersion    : %u\n",     stInfo->protVersion);
    vos_printLog(VOS_LOG_INFO,"msgType        : %u\n",     stInfo->msgType);
    vos_printLog(VOS_LOG_INFO,"comId          : %u\n",     stInfo->comId);
    vos_printLog(VOS_LOG_INFO,"etbTopoCnt     : %u\n",     stInfo->etbTopoCnt);
    vos_printLog(VOS_LOG_INFO,"opTrnTopoCnt   : %u\n",     stInfo->opTrnTopoCnt);
    vos_printLog(VOS_LOG_INFO,"replyComId     : %u\n",     stInfo->replyComId);
    vos_printLog(VOS_LOG_INFO,"replyIpAddr    : 0x%08X\n", stInfo->replyIpAddr);
    vos_printLog(VOS_LOG_INFO,"pUserRef       : %p\n",     stInfo->pUserRef);
    vos_printLog(VOS_LOG_INFO,"resultCode     : %u\n",     stInfo->resultCode);
    vos_printLog(VOS_LOG_INFO,"srcHostURI     : %u\n",     stInfo->srcHostURI);
    vos_printLog(VOS_LOG_INFO,"destHostURI    : %u\n",     stInfo->destHostURI);
    vos_printLog(VOS_LOG_INFO,"toBehavior     : %u\n",     stInfo->toBehavior);
    vos_printLog(VOS_LOG_INFO,"serviceId      : %u\n",     stInfo->serviceId);
    vos_printLog(VOS_LOG_INFO,"------------------------\n");
}

/***********************************************************************
 * PUBLIC FUNCTIONS
 **********************************************************************/

TRDP_ERR_T eInitTrdpPdStack(void)
{

    TRDP_ERR_T err     = TRDP_NO_ERR;
    uint16_t   ui16Cnt = (uint16_t)(sizeof(stMvbDB) / sizeof(stMvbDB[0]));

    /*    Copy the packet into the internal send queue, prepare for sending.    */
    /*    If we change the data, just re-publish it    */
    #define X(TRDP_MSGID, ENABLE, MVBID ,COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB,MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
    if (TRUE == ENABLE )\
    {\
        err = tlp_publish(  appHandle,                  /*    our application identifier    */\
                            &HANDLER_PUB,               /*    our pulication identifier     */\
                            NULL, NULL, \
                            0u,\
                            COMID,                      /*    ComID to send                 */\
                            0,                          /*    local consist only            */\
                            0,\
                            ui32OwnIP,                  /*    default source IP             */\
                            DEST_IP,                    /*    where to send to              */\
                            INTERVAL,                   /*    Cycle time in us              */\
                            0,                          /*    not redundant                 */\
                            TRDP_FLAGS_NONE,            /*    Use callback for errors       */\
                            NULL,                       /*    default qos and ttl           */\
                            MSG_BUFF,                   /*    initial data                  */\
                            MSG_LEN                     /*    data size                     */\
                            );\
        if (err != TRDP_NO_ERR)\
        {\
            vos_printLogStr(VOS_LOG_ERROR, "prep pd error for COM Id \n");\
            tlc_terminate();\
        }\
        else\
        {\
            printf(".................\n");\
            err = tlp_subscribe( appHandle,                 /*    our application identifier           */\
                         &HANDLER_SUB,                /*    our subscription identifier          */\
                         NULL, pdCallback,                /*    userRef & callback function          */\
                         0u,\
                         COMID,                  /*    ComID                                */\
                         0u,                        /*    topocount: local consist only        */\
                         0u,\
                         VOS_INADDR_ANY,            /*    source IP 1                          */\
                         VOS_INADDR_ANY,            /*     */\
                         DEST_IP,                    /*    Default destination IP (or MC Group) */\
                         TRDP_FLAGS_CALLBACK,        /*   */\
                         NULL,                      /*    default interface                    */\
                         8000u,         /*    Time out in us                       */\
                         TRDP_TO_SET_TO_ZERO);\
            if (err != TRDP_NO_ERR)\
            {\
                vos_printLogStr(VOS_LOG_ERROR,"prep pd subscribe error\n");\
                tlc_terminate();\
            }\
        }\
    }

    TRDP_PD_MSG_CONFIG
    #undef X

    return err;
}


/**
 * @brief this call back function for any event for trdp Receive message
 * 
 * @param pRefCon 
 * @param appHandle our application identifier 
 * @param pMsg 
 * @param pData 
 * @param dataSize 
 */
void pdCallback(void *pRefCon,
                TRDP_APP_SESSION_T appHandle,
                const TRDP_PD_INFO_T *pMsg,
                UINT8 *pData,
                UINT32 u32DataSize)
{
    char           strRecvIp[INET_ADDRSTRLEN]    = {0};
    char           replyIpAddr[INET_ADDRSTRLEN]  = {0};
    char           cDestiIpAddr[INET_ADDRSTRLEN] = {0};
    UINT32         ui32BufSize                   = TRDP_FRAME_PERAMETER;
    TRDP_ERR_T     eErr                          = TRDP_NO_ERR;
    MVB_MASTER_CB  g_mvb_cb                      = {0};
    MVB_TX_ERR_T   build_ret                     = MVB_TX_ERR_OK;

    strGetIpInString(pMsg->srcIpAddr, strRecvIp);
    strGetIpInString(pMsg->replyIpAddr, replyIpAddr);
    strGetIpInString(pMsg->destIpAddr, cDestiIpAddr);

    if (0 != u32DataSize)
    {
        vos_printLog(VOS_LOG_INFO, "*************Trdp Pd data from Network*************\n");
        vPrintTrdpPdInfo(pMsg);
        vos_printLog(VOS_LOG_INFO, "*************Trdp Pd data from Network End*************\n");
    }

    switch (pMsg->resultCode)
    {
        case TRDP_NO_ERR:
        {
            vos_printLog(VOS_LOG_INFO, "Received COM ID:%u,SRC IP:%s Msg type:%X Data Size:%d Reply COM ID:%u Reply IP :%s Dest IP:%s    \n", pMsg->comId, strRecvIp, pMsg->msgType, u32DataSize, pMsg->replyComId, replyIpAddr,cDestiIpAddr );
            #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
                if((TRUE == ENABLE) && (pMsg->comId == COMID))\
                {\
                    if((pMsg->msgType == TRDP_MSG_PR ))\
                    {\
                        vos_printLog(VOS_LOG_INFO, "The Request has been Recived \n");\
                    }\
                    else\
                    {\
                        /* check store the length od pd messsage data */\
                        MSG_LEN = (u32DataSize < DATA_MAX) ? u32DataSize : DATA_MAX;\
                        memcpy(&MSG_BUFF, pData, MSG_LEN);\
                        build_ret = mvb_tx_build_buffer(&g_mvb_cb, MVBID, 4u, pData, (uint8_t)u32DataSize);\
                        if (build_ret != MVB_TX_ERR_OK)\
                        {\
                            fprintf(stderr, "[MVB_TX] build_buffer failed (%d)\n",(int)build_ret);\
                        }\
                        else\
                        {\
                            build_ret = mvb_tx_nonblocking_send(sock, &dst, &g_mvb_cb);\
                            if ((build_ret != MVB_TX_ERR_OK) && (build_ret != MVB_TX_ERR_WOULDBLOCK))\
                            {\
                                fprintf(stderr, "[MVB_TX] Hard send error (%d)\n",\
                                        (int)build_ret);\
                                /* Continue -- don't stop the thread on a single hard error */\
                            }\
                        }\
                        if(pMsg->msgType == TRDP_MSG_PR )\
                        {\
                            vos_printLog(VOS_LOG_INFO, "The Request Reply recived \n");\
                        }\
                    }\
                }

            TRDP_PD_MSG_CONFIG
            #undef X
        }
        break;
        case TRDP_TIMEOUT_ERR:
        {
            /* The application can decide here if old data shall be invalidated or kept    */
            printf("Default Packet timed out (ComID %d, SrcIP: %s)\n", pMsg->comId, vos_ipDotted(pMsg->srcIpAddr));
        }
        break;
        default:
        {
            printf("Error on packet received (ComID %d), err = %d\n", pMsg->comId, pMsg->resultCode);
        }
        break;
    }
}

/**
 * @brief update the Trdp Queue for sending pd data and pd reply and also update the application message buffer 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @param appHandle 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eSendTrdpPdData(trdpPdMsg_t *stPdTrdpData, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle )
{
    TRDP_ERR_T  err                         = TRDP_NO_ERR ;
    char        strDestIp[INET_ADDRSTRLEN]  = {0};
    //trdpPdMsg_t stPdTrdpData                = {0};
    msgPdErr_e  eErrTrdpUpdate              = NO_ERROR;

   // if(pu8DataBuff != NULL)
   // {
        /* extract raw data from uart to Trdp Frame structure  */
        //stPdTrdpData = stTrdpPdDataFromUartFrame(pu8DataBuff, u32DataSize);
        if ( 0 != stPdTrdpData->ui16TrdpMsgLen )
        {
            /* Update the trdp appication table according to the recive data from uart */
            eErrTrdpUpdate = eUpdateTrdpPdDataTable(stPdTrdpData);
            if(NO_ERROR != eErrTrdpUpdate)
            {
                vos_printLog(VOS_LOG_ERROR, "failed to Update the table %d  \n", eErrTrdpUpdate );
            }
            else
            {
                strGetIpInString(stPdTrdpData->stPdMsg.destIpAddr,strDestIp);
                vos_printLog(VOS_LOG_INFO, "Received COM ID %u, the MSG Type is  %x the destination IP is %s  \n", stPdTrdpData->stPdMsg.comId, stPdTrdpData->stPdMsg.msgType, strDestIp);
                /* Upate the Trdp Queue according to the recived data from serial(uart) */
                eUpdateTrdpStackQueue(stPdTrdpData, appHandle);
            }
        }
        
    //}
    return err;
}

/***********************************************************************
 * END OF FILE
 **********************************************************************/