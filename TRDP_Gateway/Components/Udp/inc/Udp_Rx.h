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
 * \file    Udp_Rx.h
 * \brief   UDP receiver and MVB frame parser — public API.
 */

#ifndef UDP_RX_H
#define UDP_RX_H

#include <stdint.h>
#include <stddef.h>
#include <signal.h>
#include "trdp_if_light.h"


/****/
/* MISRA-C 2010 Compliance Notes                                       */
/*                                                                     */
/* R2.2  — All comments use C89 block style.                           */
/* R5.2  — __attribute__((packed)) removed; manual byte-level parsing  */
/*          is used instead so no padding assumption is required.      */
/* R8.1  — All public functions return the enumerated UDP_ERR_T type.  */
/*          buf_len parameter changed from int to size_t.              */
/****/

/****/
/* CONSTANTS                                                           */
/****/
#define UDP_PORT              (5000u)
#define UDP_RX_BUFFER_SIZE    (2048u)
#define UDP_RX_POLL_MS        (5)            /* 5 ms poll timeout */
#define MVB_MAX_DATA_LEN      (256u)
#define MVB_FRAME_HEADER_SIZE (8u)        /* line + fcode + address(2) + data_len + with_crc + error_flag + reserved */
//#define PLC_EVR_P             (0x012)
//#define CCU_DMB_HB_P          (0x020)
//#define PLC_PASV_P            (0x021)
//#define PLC_HMI_1_P           (0x016)
/****/
/* ERROR CODES                                                         */
/****/

typedef enum
{
    UDP_ERR_OK       =  0,
    UDP_ERR_PARAM    = -1,
    UDP_ERR_SOCKET   = -2,
    UDP_ERR_BIND     = -3,
    UDP_ERR_RECV     = -4,
    UDP_ERR_PARSE    = -5,
    UDP_ERR_NOT_INIT = -6
} UDP_ERR_T;

typedef enum
{
    PLC_CMD_P              = 0x011,
    PLC_EVR_P              = 0x012,
    PLC_CONF_P             = 0x013,
    PLC_PARAM_P            = 0x014,
    CCU_DMA_HB_P           = 0x015,
    PLC_HMI_1_P            = 0x016,
    PLC_CT_P               = 0x017,
    PLC_OUT_A_P            = 0x018,
    PLC_OUT_B_P            = 0x019,
    PLC_PR1_P              = 0x01A,
    PLC_PR2_P              = 0x01B,
    PLC_DIAG1_P            = 0x01C,
    PLC_DIAG2_P            = 0x01D,
    PLC_DIAG3_P            = 0x01E,
    PLC_DIAG4_P            = 0x01F,
    CCU_DMB_HB_P           = 0x020,
    PLC_PASV_P             = 0x021,
    PLC_HMI_3_P            = 0x022,
    PLC_PR3_P              = 0x023,
    PLC_TestTract_P        = 0x024,
    PLC_CT_P2              = 0x025,
    PLC_HMI_D_P            = 0x026,
    PLC_HMI_2_P            = 0x027,
    PLC_EVR_2_P            = 0x028,
    HMI_DMA_STS_P          = 0x030,
    HMI_DMA_PARAM1_P       = 0x031,
    HMI_DMA_CF_P           = 0x032,
    HMI_DMB_STS_P          = 0x033,
    HMI_DMB_PARAM1_P       = 0x034,
    HMI_DMB_CF_P           = 0x035,
    ACC_DMA_P              = 0x041,
    ACC_DMB_P              = 0x049,
    EVR_DMA_STS_P          = 0x051,
    EVR_DMA_CFG_P          = 0x052,
    ACU_MCA_STS_P          = 0x055,
    ACU_MCB_STS_P          = 0x059,
    PPP_DMA_STS_P          = 0x061,
    PPP_DMA_SW1_P          = 0x062,
    PPP_DMA_SW2_P          = 0x063,
    PPP_DMA_CCTV_STS_P     = 0x064,
    PPP_DMA_VE_STS_P       = 0x065,
    B_DMA_CONF_P           = 0x071,
    B_DMA_STS_P            = 0x072,
    B_DMB_CONF_P           = 0x079,
    B_DMB_STS_P            = 0x07A,
    B_TCA_CONF_P           = 0x081,
    B_TCA_STS_P            = 0x082,
    B_TCB_CONF_P           = 0x089,
    B_TCB_STS_P            = 0x08A,
    B_MCA_CONF_P           = 0x091,
    B_MCA_STS_P            = 0x092,
    B_MCB_CONF_P           = 0x099,
    B_MCB_STS_P            = 0x09A,
    ACS1_DMA_P             = 0x0A1,
    ACS2_DMA_P             = 0x0A5,
    ACS1_DMB_P             = 0x0A9,
    ACS2_DMB_P             = 0x0AD,
    ACS1_TCA_P             = 0x0B1,
    ACS2_TCA_P             = 0x0B5,
    ACS1_TCB_P             = 0x0B9,
    ACS2_TCB_P             = 0x0BD,
    ACS1_MCA_P             = 0x0C1,
    ACS2_MCA_P             = 0x0C5,
    ACS1_MCB_P             = 0x0C9,
    ACS2_MCB_P             = 0x0CD,
    T1_DMA_STS_P           = 0x0D1,
    T1_DMA_DIAG_P          = 0x0D2,
    T2_DMA_STS_P           = 0x0D5,
    T2_DMA_DIAG_P          = 0x0D6,
    T1_DMB_STS_P           = 0x0D9,
    T1_DMB_DIAG_P          = 0x0DA,
    T2_DMB_STS_P           = 0x0DD,
    T2_DMB_DIAG_P          = 0x0DE,
    T1_MCA_STS_P           = 0x0E1,
    T1_MCA_DIAG_P          = 0x0E2,
    T2_MCA_STS_P           = 0x0E5,
    T2_MCA_DIAG_P          = 0x0E6,
    T1_MCB_STS_P           = 0x0E9,
    T1_MCB_DIAG_P          = 0x0EA,
    T2_MCB_STS_P           = 0x0ED,
    T2_MCB_DIAG_P          = 0x0EE,
    DO_D1_DMA_P            = 0x101,
    DO_D2_DMA_P            = 0x102,
    DO_D3_DMA_P            = 0x103,
    DO_D4_DMA_P            = 0x104,
    IO1_DMA_INPUT1_P       = 0x111,
    IO1_DMA_INPUT2_P       = 0x112,
    IO1_DMA_INPUT3_P       = 0x113,
    IO2_DMA_INPUT1_P       = 0x121,
    IO2_DMA_INPUT2_P       = 0x122,
    IO2_DMA_INPUT3_P       = 0x123,
    DO_D1_TCA_P            = 0x201,
    DO_D2_TCA_P            = 0x202,
    DO_D3_TCA_P            = 0x203,
    DO_D4_TCA_P            = 0x204,
    IO33_TCA_INPUT1_P      = 0x211,
    IO34_TCA_INPUT1_P      = 0x221,
    DO_D1_MCA_P            = 0x301,
    DO_D2_MCA_P            = 0x302,
    DO_D3_MCA_P            = 0x303,
    DO_D4_MCA_P            = 0x304,
    IO33_MCA_INPUT1_P      = 0x311,
    IO34_MCA_INPUT1_P      = 0x321,
    DO_D1_MCB_P            = 0x401,
    DO_D2_MCB_P            = 0x402,
    DO_D3_MCB_P            = 0x403,
    DO_D4_MCB_P            = 0x404,
    IO33_MCB_INPUT1_P      = 0x411,
    IO34_MCB_INPUT1_P      = 0x421,
    DO_D1_TCB_P            = 0x501,
    DO_D2_TCB_P            = 0x502,
    DO_D3_TCB_P            = 0x503,
    DO_D4_TCB_P            = 0x504,
    IO33_TCB_INPUT1_P      = 0x511,
    IO34_TCB_INPUT1_P      = 0x521,
    DO_D1_DMB_P            = 0x601,
    DO_D2_DMB_P            = 0x602,
    DO_D3_DMB_P            = 0x603,
    DO_D4_DMB_P            = 0x604,
    DO_D5_DMB_P            = 0x605,
    DO_D6_DMB_P            = 0x606,
    DO_D7_DMB_P            = 0x607,
    DO_D8_DMB_P            = 0x608,
    IO1_DMB_INPUT1_P       = 0x611,
    IO1_DMB_INPUT2_P       = 0x612,
    IO1_DMB_INPUT3_P       = 0x613,
    IO2_DMB_INPUT1_P       = 0x621,
    IO2_DMB_INPUT2_P       = 0x622,
    IO2_DMB_INPUT3_P       = 0x623

} PortId_t;

/****/
/* MVB ACQUISITION FRAME                                               */
/*                                                                     */
/* This struct is NOT packed. The receiver parses the raw UDP byte     */
/* stream manually (byte by byte) so layout does not matter.           */
/* Field order here is logical, not on-wire.                           */
/*                                                                     */
/* On-wire byte order:                                                 */
/*   Byte 0      : line                                                */
/*   Byte 1      : fcode                                               */
/*   Bytes 2-3   : address  (little-endian uint16)                     */
/*   Byte 4      : data_len                                            */
/*   Byte 5      : with_crc                                            */
/*   Byte 6      : error_flag                                          */
/*   Byte 7      : reserved                                            */
/*   Bytes 8..N  : data[0..data_len-1]                                 */
/****/
typedef struct
{
    uint8_t  line;        /* 0 = A, 1 = B                               */
    uint8_t  fcode;       /* Function code                              */
    uint16_t address;     /* Little-endian on wire, host order here     */
    uint8_t  data_len;    /* Effective payload length (0..MVB_MAX_DATA_LEN) */
    uint8_t  with_crc;    /* 0 or 1                                     */
    uint8_t  error_flag;  /* Bitmask                                    */
    uint8_t  reserved;    /* Reserved — always 0                        */
    uint8_t  data[MVB_MAX_DATA_LEN];
} MVB_ACQUISITION_FRAME;

typedef struct mvbDataBase
{
    const uint32_t ui32MvbId;
    const uint32_t ui32Comid;
    uint8_t        ui8RecData[MVB_MAX_DATA_LEN];
    uint8_t        ui8MesgLen;
    TRDP_PUB_T     stPubHandle;
    uint8_t        ui8NewData;
} mvbDataBase_t;

#define STMVBDB_SIZE  129U
extern mvbDataBase_t stMvbDB[STMVBDB_SIZE];

/****/
/* PUBLIC FUNCTION PROTOTYPES                                          */
/****/

/**
 * @brief  Initialise the UDP receiver socket and bind to the given port.
 *
 * @param  port   UDP port number to listen on.
 * @return UDP_ERR_OK      on success.
 *         UDP_ERR_SOCKET  if socket() fails.
 *         UDP_ERR_BIND    if bind() fails.
 */
UDP_ERR_T udp_init(uint16_t port);

/**
 * @brief  Receive a single UDP datagram (non-blocking, poll-based).
 *
 * @param  buf      Pointer to caller-owned receive buffer.
 * @param  buf_len  Size of buf in bytes.
 * @return UDP_ERR_OK      and writes 0 into *rx_len when no data is ready.
 *         UDP_ERR_NOT_INIT if the socket has not been initialised.
 *         UDP_ERR_RECV     on a socket error.
 *
 * On success the actual byte count is returned via the out-parameter
 * rx_len.  The function itself always returns a UDP_ERR_T.
 */
UDP_ERR_T udp_receive(const uint8_t *buf, size_t buf_len, size_t *rx_len);

/**
 * @brief  Close the UDP socket.  Safe to call even if not initialised.
 */
void udp_close(void);

/**
 * @brief  Parse a raw UDP payload into an MVB_ACQUISITION_FRAME.
 *
 * The function performs full bounds-checking.  It does NOT rely on
 * struct packing; every field is extracted byte-by-byte from udp_buf.
 *
 * @param  udp_buf  Pointer to the received UDP payload.
 * @param  udp_len  Total length of that payload in bytes.
 * @param  frame    Pointer to the caller-owned output structure.
 * @return UDP_ERR_OK    on success.
 *         UDP_ERR_PARAM if any pointer is NULL.
 *         UDP_ERR_PARSE if the payload is too short or data_len is out of range.
 */
UDP_ERR_T parse_udp_to_mvb_frame(const uint8_t *udp_buf,
                                 size_t          udp_len,
                                 MVB_ACQUISITION_FRAME *frame);

/**
 * @brief  Print every field of an MVB frame to stdout (debug helper).
 *
 * @param  frame  Pointer to a fully parsed MVB_ACQUISITION_FRAME.
 */
void dump_mvb_frame(const MVB_ACQUISITION_FRAME *frame);

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void *pvUdpThread(void *arg);

/**
 * @brief 
 * 
 * @param frame 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eSendMvbToTrdp(MVB_ACQUISITION_FRAME *frame);

#endif /* UDP_RX_H */