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
#include "GW_trdp_md.h"
#include "GW_trdp_app.h"
#include "Udp_Rx.h"

/************************************************************************
 * Define Macros
************************************************************************/
/* We use dynamic memory    */
#define RESERVED_MEMORY     640000
#define COM_ID_OFFSET       1000

/************************************************************************
 * Define Enumeration/Structure/Unions
************************************************************************/


/************************************************************************
 * Define Private Variables
************************************************************************/
/* Pd Configuration */
TRDP_IP_ADDR_T ui32SrcIpList[MAX_PD_COMID] = {
    0x0a00006e, 0x0A000066, 0x0A000071, 0x0A000072, 0x0A000073,
    0x0A000074, 0x0A000075, 0x0A000076, 0x0A000077, 0x0A000078,
    0x0A000079, 0x0A00007A, 0, 0x0A00007C, 0x0A00007D
};
const PortId_t g_PortIdList[STMVBDB_SIZE] =
{
    PLC_CMD_P,
    PLC_EVR_P,
    PLC_CONF_P,
    PLC_PARAM_P,
    CCU_DMA_HB_P,
    PLC_HMI_1_P,
    PLC_CT_P,
    PLC_OUT_A_P,
    PLC_OUT_B_P,
    PLC_PR1_P,
    PLC_PR2_P,
    PLC_DIAG1_P,
    PLC_DIAG2_P,
    PLC_DIAG3_P,
    PLC_DIAG4_P,
    CCU_DMB_HB_P,
    PLC_PASV_P,
    PLC_HMI_3_P,
    PLC_PR3_P,
    PLC_TestTract_P,
    PLC_CT_P2,
    PLC_HMI_D_P,
    PLC_HMI_2_P,
    PLC_EVR_2_P,
    HMI_DMA_STS_P,
    HMI_DMA_PARAM1_P,
    HMI_DMA_CF_P,
    HMI_DMB_STS_P,
    HMI_DMB_PARAM1_P,
    HMI_DMB_CF_P,
    ACC_DMA_P,
    ACC_DMB_P,
    EVR_DMA_STS_P,
    EVR_DMA_CFG_P,
    ACU_MCA_STS_P,
    ACU_MCB_STS_P,
    PPP_DMA_STS_P,
    PPP_DMA_SW1_P,
    PPP_DMA_SW2_P,
    PPP_DMA_CCTV_STS_P,
    PPP_DMA_VE_STS_P,
    B_DMA_CONF_P,
    B_DMA_STS_P,
    B_DMB_CONF_P,
    B_DMB_STS_P,
    B_TCA_CONF_P,
    B_TCA_STS_P,
    B_TCB_CONF_P,
    B_TCB_STS_P,
    B_MCA_CONF_P,
    B_MCA_STS_P,
    B_MCB_CONF_P,
    B_MCB_STS_P,
    ACS1_DMA_P,
    ACS2_DMA_P,
    ACS1_DMB_P,
    ACS2_DMB_P,
    ACS1_TCA_P,
    ACS2_TCA_P,
    ACS1_TCB_P,
    ACS2_TCB_P,
    ACS1_MCA_P,
    ACS2_MCA_P,
    ACS1_MCB_P,
    ACS2_MCB_P,
    T1_DMA_STS_P,
    T1_DMA_DIAG_P,
    T2_DMA_STS_P,
    T2_DMA_DIAG_P,
    T1_DMB_STS_P,
    T1_DMB_DIAG_P,
    T2_DMB_STS_P,
    T2_DMB_DIAG_P,
    T1_MCA_STS_P,
    T1_MCA_DIAG_P,
    T2_MCA_STS_P,
    T2_MCA_DIAG_P,
    T1_MCB_STS_P,
    T1_MCB_DIAG_P,
    T2_MCB_STS_P,
    T2_MCB_DIAG_P,
    DO_D1_DMA_P,
    DO_D2_DMA_P,
    DO_D3_DMA_P,
    DO_D4_DMA_P,
    IO1_DMA_INPUT1_P,
    IO1_DMA_INPUT2_P,
    IO1_DMA_INPUT3_P,
    IO2_DMA_INPUT1_P,
    IO2_DMA_INPUT2_P,
    IO2_DMA_INPUT3_P,
    DO_D1_TCA_P,
    DO_D2_TCA_P,
    DO_D3_TCA_P,
    DO_D4_TCA_P,
    IO33_TCA_INPUT1_P,
    IO34_TCA_INPUT1_P,
    DO_D1_MCA_P,
    DO_D2_MCA_P,
    DO_D3_MCA_P,
    DO_D4_MCA_P,
    IO33_MCA_INPUT1_P,
    IO34_MCA_INPUT1_P,
    DO_D1_MCB_P,
    DO_D2_MCB_P,
    DO_D3_MCB_P,
    DO_D4_MCB_P,
    IO33_MCB_INPUT1_P,
    IO34_MCB_INPUT1_P,
    DO_D1_TCB_P,
    DO_D2_TCB_P,
    DO_D3_TCB_P,
    DO_D4_TCB_P,
    IO33_TCB_INPUT1_P,
    IO34_TCB_INPUT1_P,
    DO_D1_DMB_P,
    DO_D2_DMB_P,
    DO_D3_DMB_P,
    DO_D4_DMB_P,
    DO_D5_DMB_P,
    DO_D6_DMB_P,
    DO_D7_DMB_P,
    DO_D8_DMB_P,
    IO1_DMB_INPUT1_P,
    IO1_DMB_INPUT2_P,
    IO1_DMB_INPUT3_P,
    IO2_DMB_INPUT1_P,
    IO2_DMB_INPUT2_P,
    IO2_DMB_INPUT3_P
};

TRDP_IP_ADDR_T ui32SrcIpUpperList[MAX_PD_COMID] = {
    0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e,
    0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e,
    0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e, 0x0a00007e
};

UINT32 ui32ComIdList[STMVBDB_SIZE] = {
PLC_CMD_P          + COM_ID_OFFSET,
PLC_EVR_P          + COM_ID_OFFSET,
PLC_CONF_P         + COM_ID_OFFSET,
PLC_PARAM_P        + COM_ID_OFFSET,
CCU_DMA_HB_P       + COM_ID_OFFSET,
PLC_HMI_1_P        + COM_ID_OFFSET,
PLC_CT_P           + COM_ID_OFFSET,
PLC_OUT_A_P        + COM_ID_OFFSET,
PLC_OUT_B_P        + COM_ID_OFFSET,
PLC_PR1_P          + COM_ID_OFFSET,
PLC_PR2_P          + COM_ID_OFFSET,
PLC_DIAG1_P        + COM_ID_OFFSET,
PLC_DIAG2_P        + COM_ID_OFFSET,
PLC_DIAG3_P        + COM_ID_OFFSET,
PLC_DIAG4_P        + COM_ID_OFFSET,
CCU_DMB_HB_P       + COM_ID_OFFSET,
PLC_PASV_P         + COM_ID_OFFSET,
PLC_HMI_3_P        + COM_ID_OFFSET,
PLC_PR3_P          + COM_ID_OFFSET,
PLC_TestTract_P    + COM_ID_OFFSET,
PLC_CT_P2          + COM_ID_OFFSET,
PLC_HMI_D_P        + COM_ID_OFFSET,
PLC_HMI_2_P        + COM_ID_OFFSET,
PLC_EVR_2_P        + COM_ID_OFFSET,
HMI_DMA_STS_P      + COM_ID_OFFSET,
HMI_DMA_PARAM1_P   + COM_ID_OFFSET,
HMI_DMA_CF_P       + COM_ID_OFFSET,
HMI_DMB_STS_P      + COM_ID_OFFSET,
HMI_DMB_PARAM1_P   + COM_ID_OFFSET,
HMI_DMB_CF_P       + COM_ID_OFFSET,
ACC_DMA_P          + COM_ID_OFFSET,
ACC_DMB_P          + COM_ID_OFFSET,
EVR_DMA_STS_P      + COM_ID_OFFSET,
EVR_DMA_CFG_P      + COM_ID_OFFSET,
ACU_MCA_STS_P      + COM_ID_OFFSET,
ACU_MCB_STS_P      + COM_ID_OFFSET,
PPP_DMA_STS_P      + COM_ID_OFFSET,
PPP_DMA_SW1_P      + COM_ID_OFFSET,
PPP_DMA_SW2_P      + COM_ID_OFFSET,
PPP_DMA_CCTV_STS_P + COM_ID_OFFSET ,
PPP_DMA_VE_STS_P   + COM_ID_OFFSET,
B_DMA_CONF_P       + COM_ID_OFFSET,
B_DMA_STS_P        + COM_ID_OFFSET,
B_DMB_CONF_P       + COM_ID_OFFSET,
B_DMB_STS_P        + COM_ID_OFFSET,
B_TCA_CONF_P       + COM_ID_OFFSET,
B_TCA_STS_P        + COM_ID_OFFSET,
B_TCB_CONF_P       + COM_ID_OFFSET,
B_TCB_STS_P        + COM_ID_OFFSET,
B_MCA_CONF_P       + COM_ID_OFFSET,
B_MCA_STS_P        + COM_ID_OFFSET,
B_MCB_CONF_P       + COM_ID_OFFSET,
B_MCB_STS_P        + COM_ID_OFFSET,
ACS1_DMA_P         + COM_ID_OFFSET,
ACS2_DMA_P         + COM_ID_OFFSET,
ACS1_DMB_P         + COM_ID_OFFSET,
ACS2_DMB_P         + COM_ID_OFFSET,
ACS1_TCA_P         + COM_ID_OFFSET,
ACS2_TCA_P         + COM_ID_OFFSET,
ACS1_TCB_P         + COM_ID_OFFSET,
ACS2_TCB_P         + COM_ID_OFFSET,
ACS1_MCA_P         + COM_ID_OFFSET,
ACS2_MCA_P         + COM_ID_OFFSET,
ACS1_MCB_P         + COM_ID_OFFSET,
ACS2_MCB_P         + COM_ID_OFFSET,
T1_DMA_STS_P       + COM_ID_OFFSET,
T1_DMA_DIAG_P      + COM_ID_OFFSET,
T2_DMA_STS_P       + COM_ID_OFFSET,
T2_DMA_DIAG_P      + COM_ID_OFFSET,
T1_DMB_STS_P       + COM_ID_OFFSET,
T1_DMB_DIAG_P      + COM_ID_OFFSET,
T2_DMB_STS_P       + COM_ID_OFFSET,
T2_DMB_DIAG_P      + COM_ID_OFFSET,
T1_MCA_STS_P       + COM_ID_OFFSET,
T1_MCA_DIAG_P      + COM_ID_OFFSET,
T2_MCA_STS_P       + COM_ID_OFFSET,
T2_MCA_DIAG_P      + COM_ID_OFFSET,
T1_MCB_STS_P       + COM_ID_OFFSET,
T1_MCB_DIAG_P      + COM_ID_OFFSET,
T2_MCB_STS_P       + COM_ID_OFFSET,
T2_MCB_DIAG_P      + COM_ID_OFFSET,
DO_D1_DMA_P        + COM_ID_OFFSET,
DO_D2_DMA_P        + COM_ID_OFFSET,
DO_D3_DMA_P        + COM_ID_OFFSET,
DO_D4_DMA_P        + COM_ID_OFFSET,
IO1_DMA_INPUT1_P   + COM_ID_OFFSET,
IO1_DMA_INPUT2_P   + COM_ID_OFFSET,
IO1_DMA_INPUT3_P   + COM_ID_OFFSET,
IO2_DMA_INPUT1_P   + COM_ID_OFFSET,
IO2_DMA_INPUT2_P   + COM_ID_OFFSET,
IO2_DMA_INPUT3_P   + COM_ID_OFFSET,
DO_D1_TCA_P        + COM_ID_OFFSET,
DO_D2_TCA_P        + COM_ID_OFFSET,
DO_D3_TCA_P        + COM_ID_OFFSET,
DO_D4_TCA_P        + COM_ID_OFFSET,
IO33_TCA_INPUT1_P  + COM_ID_OFFSET,
IO34_TCA_INPUT1_P  + COM_ID_OFFSET,
DO_D1_MCA_P        + COM_ID_OFFSET,
DO_D2_MCA_P        + COM_ID_OFFSET,
DO_D3_MCA_P        + COM_ID_OFFSET,
DO_D4_MCA_P        + COM_ID_OFFSET,
IO33_MCA_INPUT1_P  + COM_ID_OFFSET,
IO34_MCA_INPUT1_P  + COM_ID_OFFSET,
DO_D1_MCB_P        + COM_ID_OFFSET,
DO_D2_MCB_P        + COM_ID_OFFSET,
DO_D3_MCB_P        + COM_ID_OFFSET,
DO_D4_MCB_P        + COM_ID_OFFSET,
IO33_MCB_INPUT1_P  + COM_ID_OFFSET,
IO34_MCB_INPUT1_P  + COM_ID_OFFSET,
DO_D1_TCB_P        + COM_ID_OFFSET,
DO_D2_TCB_P        + COM_ID_OFFSET,
DO_D3_TCB_P        + COM_ID_OFFSET,
DO_D4_TCB_P        + COM_ID_OFFSET,
IO33_TCB_INPUT1_P  + COM_ID_OFFSET,
IO34_TCB_INPUT1_P  + COM_ID_OFFSET,
DO_D1_DMB_P        + COM_ID_OFFSET,
DO_D2_DMB_P        + COM_ID_OFFSET,
DO_D3_DMB_P        + COM_ID_OFFSET,
DO_D4_DMB_P        + COM_ID_OFFSET,
DO_D5_DMB_P        + COM_ID_OFFSET,
DO_D6_DMB_P        + COM_ID_OFFSET,
DO_D7_DMB_P        + COM_ID_OFFSET,
DO_D8_DMB_P        + COM_ID_OFFSET,
IO1_DMB_INPUT1_P   + COM_ID_OFFSET,
IO1_DMB_INPUT2_P   + COM_ID_OFFSET,
IO1_DMB_INPUT3_P   + COM_ID_OFFSET,
IO2_DMB_INPUT1_P   + COM_ID_OFFSET,
IO2_DMB_INPUT2_P   + COM_ID_OFFSET,
IO2_DMB_INPUT3_P   + COM_ID_OFFSET
};

/* Source IP addresses to receive from - modify as needed */
TRDP_IP_ADDR_T gDestIpList[MAX_PD_COMID] = {
    0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e,
    0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e,
    0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e, 0x0a00001e
};

/* MD Configuration  */
TRDP_IP_ADDR_T gDestIpMDList[MAX_MD_COMID] = { 0xE0A2A84B, 0xE0A2A84C, 0xE0A2A84E, 0, 0xE0A2A84F};
UINT32 ui32ComIdMDList[MAX_MD_COMID]       = {1001, 1000, 1002, 1003, 1004};

/* Source IP addresses to receive from - modify as needed */
TRDP_IP_ADDR_T ui32SrcMDIpList[MAX_MD_COMID]    = { 0x0A0000A0, 0x0A0000A0, 0x0A0000A0, 0x0A0000A0, 0x0A0000A0 };
TRDP_IP_ADDR_T ui32SrcMDLowIpList[MAX_MD_COMID] = { 0x0A000065, 0x0A000065, 0x0A000065, 0x0A000065, 0x0A000065 };

TRDP_COMID_DSID_MAP_T   gComIdMap[] =
{
    {PD_DATA_SET, PD_DATA_SET},
    {MD_DATA_SET, MD_DATA_SET}
};

TRDP_DATASET_T  gDataSetPd =
{
    PD_DATA_SET,          /*    dataset/com ID  */
    0,                    /*    reserved        */
    PD_DATA_FRAME_MEMBER, /*    No of elements, var size    */
    {'\0'},               /*    name */
    {                     /*    TRDP_DATASET_ELEMENT_T[]    */
        {
            TRDP_UINT32,    /**< Field 1: source IP address for filtering    offset 0                        */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< Field 2: destination IP address for filtering     offset 4                   */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< Field 3: sequence counter                        offset 8                    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT16,    /**< Field 4: Protocol version                        offset 12                    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT16,    /**<  Field 5 :Protocol ('PD', 'MD', ...)                 offset 14                 */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**<  Field 6 :ComID                                      offset 16                 */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**<  Field 7 :received ETB topocount                        offset 20              */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**<  Field 8 :received operational train directory topocount       offset 24       */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**<  Field 9 :ComID for reply (request only)                 offset 28             */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< Field 10 :IP address for reply (request only)             offset 32            */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**<  Field 11: User reference given with the local subscribe      offset 36         */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_INT32,    /**< Field 12: error code                                       offset 40           */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_CHAR8,    /**< Field 13: source URI host part (unused)                      offset 44         */
            81,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_CHAR8,    /**< Field 14: destination URI host part (unused)                  offset  125       */
            81,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< Field 15: callback can decide about handling of data on timeout    offset 206    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< Field 16: the reserved field of the PD header                  offset 0    214   */
            1,
            NULL, NULL, 0, 0, NULL
        }
    }
};

TRDP_DATASET_T  gDataSetMd =
{
    MD_DATA_SET,          /*    dataset/com ID  */
    0,                    /*    reserved        */
    MD_DATA_FRAME_MEMBER, /*    No of elements, var size    */
    {'\0'},               /*    name */
    {                     /*    TRDP_DATASET_ELEMENT_T[]    */
        {
            TRDP_UINT32,    /**< source IP address for filtering          offset 0    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< destination IP address for filtering     offset 1    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< sequence counter                         offset 2     */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT16,    /**< Protocol version                         offset 3      */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT16,    /**< Protocol ('PD', 'MD', ...)                offset 4      */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< ComID                                     offset 5      */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< received topocount                        offset 6     */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< received topocount                        offset 7       */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_CHAR8,    /**< session is about to die                    offset 8        */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< number of ReplyQuery received             offset 9     */
            1,
            NULL, NULL, 0, 0, NULL
        }
        ,
        {
            TRDP_UINT32,    /**< number of Confirm sent                   offset 10      */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< number of Confirm Timeouts (incremented by listeners    offset 11   */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT16,    /**< error code, user stat                                   offset 12   */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_INT32,    /**< reply status                                            offset 13    */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT8,    /**< for response                                           offset 14     */
            16,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< reply timeout in us given with the request             offset 15      */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT8,    /**< source URI user part from MD header                     offset 16       */
            33,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT8,    /**< for response                                            offset 17        */
            81,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT8,    /**< destination URI user part from MD header               offset 18         */
            33,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT8,    /**< destination URI host part (unused)                     offset 19         */
            81,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< number of expected replies, 0 if unknown              offset 20          */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< actual number of replies for the request             offset 21           */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_UINT32,    /**< User reference given with the local call              offset 22            */
            1,
            NULL, NULL, 0, 0, NULL
        },
        {
            TRDP_INT32,    /**< error code                                           offset 23             */
            1,
            NULL, NULL, 0, 0, NULL
        }
    }
};

TRDP_DATASET_T  *gDataSets[] =
{
    &gDataSetPd,
    &gDataSetMd
};

UINT32  *refCon = NULL;

/************************************************************************
 * Define Global Variables
************************************************************************/

UINT32  ui32OwnIP = 0u;
TRDP_APP_SESSION_T appHandle;

/**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
**********************************************************************/

/**
 * @brief 
 * 
 */
static void vSetDefTrdpMdMsgConfig(void);

/**
 * @brief initialized the Trdp Stack according to Trdp Configuration
 * 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eTrdpInit(void);

/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/

/**
 * @brief To set default Configruation
 * 
 */
static void vSetDefTrdpPdMsgConfig(void)
{
    const char strDefMsg[25] = "ready to send data!!";

    #define X(TRDP_MSGID, ENABLE, MVBID, COMID , INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
        ENABLE = TRUE;\
        COMID = ui32ComIdList[TRDP_MSGID];\
        MVBID = g_PortIdList[TRDP_MSGID];\
        SRCIP_1 = 0/*ui32SrcIpList[TRDP_MSGID]*/;\
        SRCIP_2 = 0/*ui32SrcIpUpperList[TRDP_MSGID]*/;\
        DEST_IP = 0x0A00001E/*0x0A0000EE*/;\
        MSG_TYPE = MSG_TYPE_PD_REQUEST;\
        HANDLER_PUB = NULL;\
        HANDLER_SUB = NULL;\
       /*memcpy(MSG_BUFF, strDefMsg, strlen(strDefMsg));*/\
        /*MSG_LEN = strlen(strDefMsg);*/\
        INTERVAL = 10000u;\

        TRDP_PD_MSG_CONFIG
    #undef X
}

 /**
 * @brief Set default MD configuration
 */
static void vSetDefTrdpMdMsgConfig(void)
{
    const char strDefMsg[50] = "MD Ready - Default Response";

        #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP, MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
            ENABLE          = TRUE;\
            COMID           = ui32ComIdMDList[TRDP_MD_MSG_ID];\
            SRC_LOW_IP      = ui32SrcMDLowIpList[TRDP_MD_MSG_ID];\
            SRC_HIGH_IP     = ui32SrcMDIpList[TRDP_MD_MSG_ID];\
            DEST_IP         = gDestIpMDList[TRDP_MD_MSG_ID];\
            MD_HANDLER      = NULL;\
            TIMEOUT         = 5000000u;  /* 5 seconds default */\
            MSG_TYPE        = MSG_TYPE_MD_REQUEST;\
            CONFIRM_REQUIRE = FALSE;\
            memcpy(MSG_REQ_BUFF.pui8TrdpMsg, strDefMsg, strlen(strDefMsg));\
            MSG_REQ_BUFF.ui32TrdpMsgLen = strlen(strDefMsg);\

        TRDP_MD_MSG_CONFIG
    #undef Y
}

/**
  * @brief For printng the default configuartion for Trdp Communication
  * 
  */
static void vPdConfigPrint(void)
{
    char strSrcIp[INET_ADDRSTRLEN]   = {0};
    char strDestIp[INET_ADDRSTRLEN]  = {0};

    vos_printLog(VOS_LOG_INFO, "*************** PD Configuration ***************\n");
    #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
        if (ENABLE == TRUE)\
        {\
            strGetIpInString(DEST_IP, strDestIp);\
            strGetIpInString(SRCIP_1, strSrcIp);\
            vos_printLog(VOS_LOG_INFO, "The Config: %02d The ComId: %u and MVB id: %u, DestIp: %-15s, SrcIp: %-15s, msgType is %X \n",TRDP_MSGID, COMID, MVBID, strDestIp, strSrcIp, MSG_TYPE);\
        }\

        TRDP_PD_MSG_CONFIG
    #undef X
    vos_printLog(VOS_LOG_INFO, "*******************************************************\n");

}

 /**
 * @brief Print MD configuration
 */
static void vMdConfigPrint(void)
{
    char strSrcLowIp[INET_ADDRSTRLEN]    = {0};
    char strSrcHighIp[INET_ADDRSTRLEN]   = {0};
    char strMultiCastIp[INET_ADDRSTRLEN] = {0};

    vos_printLog(VOS_LOG_INFO, "*************** MD Configuration ***************\n");
    #define Y(TRDP_MD_MSG_ID, ENABLE, COMID, TIMEOUT, DEST_IP, SRC_LOW_IP, SRC_HIGH_IP,  MD_HANDLER, MSG_BUFF, MSG_REQ_BUFF, MSG_TYPE, CONFIRM_REQUIRE )\
        strGetIpInString(DEST_IP, strMultiCastIp);\
        strGetIpInString(SRC_LOW_IP, strSrcLowIp);\
        strGetIpInString(SRC_HIGH_IP, strSrcHighIp);\
        vos_printLog(VOS_LOG_INFO,\
                        "MD Config[%02d]: COM ID: %u, multicat Ip: %-15s, SrcLowIP: %-15s, SrcHighIP: %-15s, Type: %X,\n",\
                        TRDP_MD_MSG_ID,\
                        COMID,\
                        strMultiCastIp,\
                        strSrcLowIp,\
                        strSrcHighIp,\
                        MSG_TYPE\
                        );\

        TRDP_MD_MSG_CONFIG
    #undef Y

    vos_printLog(VOS_LOG_INFO, "*******************************************************\n");
}

  /**
  * @brief Get the memory statictis 
  * 
  */
void vGetMemoryDetail(void)
{
    TRDP_STATISTICS_T stats;
    TRDP_ERR_T err;

    err = tlc_getStatistics(appHandle, &stats);
    if (err == TRDP_NO_ERR)
    {
        printf("DEBUG: Memory Statistics:\n");
        printf("  Total blocks allocated: %u\n", stats.mem.total);
        printf("  Allocation errors: %u\n", stats.mem.numAllocErr);
        printf("  Free errors: %u\n", stats.mem.numFreeErr);
        printf("  Block size: %ls bytes\n", stats.mem.blockSize);
        printf("  Number of blocks: %u\n", stats.mem.numAllocBlocks);
    }
}

/**
 * @brief initialized the Trdp Stack according to Trdp Configuration
 * 
 * @return TRDP_ERR_T 
 */
static TRDP_ERR_T eTrdpInit(void)
{
    TRDP_ERR_T             eErr            = TRDP_NO_ERR;
    TRDP_PD_CONFIG_T       pdConfiguration = {pdCallback , NULL, TRDP_PD_DEFAULT_SEND_PARAM, TRDP_FLAGS_CALLBACK, 10000000, TRDP_TO_SET_TO_ZERO, 0};
    TRDP_MD_CONFIG_T       mdConfiguration = {mdCallback, NULL, {3u, 64u, 2u, 0u, 0u}, TRDP_FLAGS_CALLBACK, 1000000u, 1000000u, 1000000u, 1000000u, 17225u, 17225u, 64 };
    TRDP_PROCESS_CONFIG_T  processConfig   = {"TrdpGateway", "", "", TRDP_PROCESS_DEFAULT_CYCLE_TIME, 0u, TRDP_OPTION_BLOCK};
    TRDP_MARSHALL_CONFIG_T stMarshall      = {tau_marshall, tau_unmarshall, 0};

    /*  intilise the marshalling  */
    eErr = tau_initMarshall((void *)&refCon, sizeof(gComIdMap)/sizeof(TRDP_COMID_DSID_MAP_T), gComIdMap, (sizeof(gDataSets) / sizeof(TRDP_DATASET_T  *)), gDataSets);
    if (eErr != TRDP_NO_ERR)
    {
        printf("Marshalling initialization error %d \n ", eErr);
    }
    else
    {
        /* to nothing  */
    }

    /*  Open a session  */
    if (tlc_openSession(&appHandle,
                        ui32OwnIP, 0,                           /* use default IP address           */
                        &stMarshall,                            /* no Marshalling                   */
                        &pdConfiguration, &mdConfiguration,     /* system defaults for PD and MD    */
                        &processConfig) != TRDP_NO_ERR)
    {
        vos_printLogStr(VOS_LOG_ERROR, "Initialization error\n");
    }

    eErr= eInitTrdpPdStack();
    if(TRDP_NO_ERR != eErr)
    {
        vos_printLog(VOS_LOG_ERROR, "Initialization eInitTrdpPdStack fail %d \n" , eErr);
    }
    else
    {
        // eErr = eInitTrdpMdStack();
        // if(TRDP_NO_ERR != eErr)
        // {
        //     vos_printLog(VOS_LOG_ERROR, "Initialization eInitTrdpMdStack fail %d \n" , eErr);
        // }
    }

    return eErr;
}

 /** callback routine for TRDP logging/error output
 *
 *  @param[in]      pRefCon         user supplied context pointer
 *  @param[in]      category        Log category (Error, Warning, Info etc.)
 *  @param[in]      pTime           pointer to NULL-terminated string of time stamp
 *  @param[in]      pFile           pointer to NULL-terminated string of source module
 *  @param[in]      LineNumber      line
 *  @param[in]      pMsgStr         pointer to NULL-terminated string
 *
 *  @retval         none
 */
void dbgOut (
    void        *pRefCon,
    TRDP_LOG_T  category,
    const CHAR8 *pTime,
    const CHAR8 *pFile,
    UINT16      LineNumber,
    const CHAR8 *pMsgStr)
{
    const char *catStr[] = {"**Error:", "Warning:", "   Info:", "  Debug:", "   User:"};

    if (category != VOS_LOG_DBG)
    {
        /* we filter some more info we're not interested in */
        if ((category != VOS_LOG_INFO) ||
            (strstr(pFile, "vos_sock.c") == NULL))
        {
            printf("%s %s %s",
                   strrchr(pTime, '-') + 1,
                   catStr[category],
                   pMsgStr);
        }
    }
}

/***********************************************************************
 * PUBLIC FUNCTIONS
 **********************************************************************/

/**
 * @brief Convert IP address from decimal to string for reader useablity
 *
 * @param ipAddress
 * @param strIp
 */
 void strGetIpInString(uint32_t ipAddress, char *strIp)
{
    struct in_addr addr = {0};

    if (ipAddress > 0)
    {
        addr.s_addr = htonl(ipAddress);
        inet_ntop(AF_INET, &addr, strIp, INET_ADDRSTRLEN);
    }
    else
    {
        strIp[0] = '\0';
    }
}

 /**
 * @brief Get the Own Ip Address object
 * 
 * @return uint32_t 
 */
uint32_t getOwnIpAddress()
{
    struct ifaddrs *ifaddr   = NULL;
    struct ifaddrs *ifa      = NULL;
    char ip[INET_ADDRSTRLEN] = {0};  // For IPv4 (use INET6_ADDRSTRLEN for IPv6)
    unsigned int  ownIp[4]   = {0};
    uint32_t uidOwnIpAdd     =  0;

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        return 1;
    }

    /* Loop through all interfaces */
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
        {
            continue;
        }

            // IPv4 only
            if (ifa->ifa_addr->sa_family == AF_INET)
            {
                struct sockaddr_in *sa = (struct sockaddr_in *) ifa->ifa_addr;
                inet_ntop(AF_INET, &(sa->sin_addr), ip, sizeof(ip));

                if(strcmp(ifa->ifa_name,"eth1") == 0 )
                {
                    if (sscanf(ip,"%u.%u.%u.%u",&ownIp[3],&ownIp[2],&ownIp[1],&ownIp[0]) < 4)
                    {
                        return 0;
                    }
                    uidOwnIpAdd = (ownIp[3] << 24) | (ownIp[2] << 16) | (ownIp[1] << 8) | ownIp[0];
                    break;
                }
            }
    }

    freeifaddrs(ifaddr);
    return uidOwnIpAdd;
}

/**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @return trdpMsgType_e 
 */
trdpMsgType_e eGetTrdpMsgType(uint8_t * pu8DataBuff)
{

    trdpMsgType_e u32MsgType = 0;
    uint16_t var = 0 ;
    const uint8_t u8PosOfMsgType = 5;

    if(NULL != pu8DataBuff)
    {
        memcpy(&var, pu8DataBuff + u8PosOfMsgType , sizeof(uint16_t));
        var == TRDP_PD ? (u32MsgType = TRDP_PD) : (u32MsgType = UNKNOWN_TYPE);
        if(u32MsgType == UNKNOWN_TYPE)
        {
            var == TRDP_MD ? (u32MsgType = TRDP_MD) : (u32MsgType = UNKNOWN_TYPE);
        }
    }
    else
    {
        u32MsgType = UNKNOWN_TYPE;
    }

    return u32MsgType;
}

/**
 * @brief 
 * 
 * @param pu8DataBuff 
 * @param u32DataSize 
 * @param appHandle 
 * @return trdpMsgType_e 
 */
trdpMsgType_e eSendTrdpData(uint8_t * pu8DataBuff, uint32_t u32DataSize, TRDP_APP_SESSION_T appHandle)
{
trdpMsgType_e eMsgType = UNKNOWN_TYPE;
TRDP_ERR_T    eErr     = TRDP_NO_ERR;

    if ((NULL != pu8DataBuff) &&  (0 != u32DataSize))
    {
        // eMsgType = eGetTrdpMsgType(pu8DataBuff);

        // vos_printLog(VOS_LOG_INFO, "The message type recived from uart is  %d \n" , eMsgType);

        // switch (eMsgType)
        // {
        //     case TRDP_PD:
        //     {
        //         eErr = eSendTrdpPdData(pu8DataBuff, u32DataSize, appHandle );
        //         vos_printLog(VOS_LOG_INFO, "Sending data on Trdp PD data %d \n", eErr);
        //         if (TRDP_NO_ERR != eErr)
        //         {
        //             vos_printLog(VOS_LOG_ERROR, "Unable to Send pd Data from uart to Trdp %d \n" , eErr);
        //         }
        //     }
        //     break;
        //     case TRDP_MD:
        //     {
        //         eErr = eSendMdMessage(pu8DataBuff, u32DataSize, appHandle );
        //         vos_printLog(VOS_LOG_INFO, "Sending data on Trdp MD data %d \n", eErr);
        //         if (TRDP_NO_ERR != eErr)
        //         {
        //             vos_printLog(VOS_LOG_ERROR, "Unable to Send Md Data from uart to Trdp %d \n" , eErr);
        //         }
        //     }
        //     break;
        //     default:
        //     {
        //         eMsgType = UNKNOWN_TYPE;
        //     }
        //     break;
        //}

       // eSendTrdpPdData(pu8DataBuff, u32DataSize, appHandle );
    }

    return eMsgType;
}

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void* pvEthTrdp(void* arg)
{
    //TRDP_APP_SESSION_T      appHandle; /*    Our identifier to the library instance    */
    TRDP_ERR_T        err             = TRDP_NO_ERR;
    TRDP_MEM_CONFIG_T dynamicConfig   = {NULL, RESERVED_MEMORY, {0}};
    int               rv              = 0;

    /*    Init the library  */
    if (tlc_init(&dbgOut,                              /* no logging    */
                 NULL,
                 &dynamicConfig) != TRDP_NO_ERR)       /* Use application supplied memory    */
    {
        printf("Initialization error\n");
    }

    /* geting own ip address */
    ui32OwnIP = getOwnIpAddress();

    /* get the default PD config  */
    vSetDefTrdpPdMsgConfig();

    /* get the default MD config  */
    vSetDefTrdpMdMsgConfig();

    /* print default MD config  */
    vMdConfigPrint();

    /* print default PD config  */
    vPdConfigPrint();

    /* intit the Trdp Stack for Pd and Md Message type according to configuration */
    if ( TRDP_NO_ERR != eTrdpInit())
    {
        vos_printLog(VOS_LOG_ERROR, "Failed to init eTrdpInit, error\n" );
    }

    /* update the trdp stack if there is any update in configuration */
    err = tlc_updateSession(appHandle);
    if (err != TRDP_NO_ERR)
    {
        vos_printLog(VOS_LOG_USR, "tlc_updateSession error (%s)\n", vos_getErrorString((VOS_ERR_T)err));
        tlc_terminate();
        printf("*******************trdp has stop \n");
    }
    while (1)
    {
        TRDP_FDS_T rfds;
        INT32 noDesc;
        TRDP_TIME_T tv;
        const TRDP_TIME_T   max_tv  = {1, 0};
        const TRDP_TIME_T   min_tv  = {0, TRDP_PROCESS_DEFAULT_CYCLE_TIME};

        /* clear the file discriptor */
        FD_ZERO(&rfds);

        /* check for the data in trdp Stack, which message is ready to send and recive */
        tlc_getInterval(appHandle, &tv, &rfds, &noDesc);

        if (vos_cmpTime(&tv, &max_tv) > 0)
        {
            tv = max_tv;
        }
        else if (vos_cmpTime(&tv, &min_tv) < 0)
        {
            tv = min_tv;
        }

        /* check for the data which every is ready, otherwise put to sleep */
        rv = vos_select(noDesc, &rfds, NULL, NULL, &tv);

        /* Process the Trdp PD message for sending and reciving */
        (void) tlc_process(appHandle, &rfds, &rv);

        /* Process the Trdp MD message for sending and reciving */
       // (void) tlm_process(appHandle, &rfds, &rv);

    }

    #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
        if (ENABLE)\
        {\
            tlp_unsubscribe(appHandle, HANDLER_SUB);\
            tlp_unpublish(appHandle, HANDLER_PUB);\
        }

        TRDP_PD_MSG_CONFIG
    #undef X

    tlc_closeSession(appHandle);
    tlc_terminate();
    vos_printLog(VOS_LOG_ERROR, "Programe has been Terminated \n" );

}

/***********************************************************************
 * END OF FILE
 **********************************************************************/