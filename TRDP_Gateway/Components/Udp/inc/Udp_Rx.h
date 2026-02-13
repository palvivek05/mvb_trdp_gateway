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
#define PLC_EVR_P             (0x012)
#define CCU_DMB_HB_P          (0x020)
#define PLC_PASV_P            (0x021)
#define PLC_HMI_1_P           (0x016)
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

#endif /* UDP_RX_H */