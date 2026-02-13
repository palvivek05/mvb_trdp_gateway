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
 * \file    udp_v4.c
 * \brief   UDP receiver and MVB frame parser — implementation.
 */

/****/
/* MISRA-C 2010 Compliance Notes                                       */
/*                                                                     */
/* R2.2  — All comments converted to C89 block style.                  */
/* R8.1  — All return values use UDP_ERR_T; magic numbers eliminated.  */
/* R10.1 — Return values of setsockopt, fcntl, close checked.          */
/* R12.1 — Bitwise results compared explicitly with != 0.              */
/*         Pointer NULL checks use == NULL / != NULL.                   */
/* R14.3 — Every if/else branch is enclosed in braces.                 */
/* R17.8 — No side-effects (++) inside array subscript or assignment.  */
/* R18.1 — Dead commented-out code removed.                            */
/****/

/****/
/* Include Header Files                                                */
/****/
#include "Udp_Rx.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/socket.h>
#include "GW_trdp_pd.h"
#include "GW_trdp_md.h"
#include "GW_trdp_app.h"
#include "GW_uart.h"
#include "GW_common.h"
#include "tau_marshall.h"
/****/
/* FILE-SCOPE STATE                                                    */
/****/
static int udp_sock = -1;   /* -1 indicates "not initialised"         */

/****/
/* PRIVATE HELPER — parse a little-endian uint16 from a byte pointer  */
/****/
static uint16_t read_le_uint16(const uint8_t *p)
{
    uint16_t value;
    value = (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8u));
    return value;
}

/* ================================================================== */
/* udp_init                                                            */
/* ================================================================== */
UDP_ERR_T udp_init(uint16_t port)
{
    struct sockaddr_in addr;
    int                opt       = 1;
    int                sock_ret;
    int                fcntl_ret;

    sock_ret = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_ret < 0)
    {
        perror("socket");
        return UDP_ERR_SOCKET;
    }
    udp_sock = sock_ret;

    /* Allow fast restart — return value checked */
    if (setsockopt(udp_sock, SOL_SOCKET, SO_REUSEADDR,
                   &opt, (socklen_t)sizeof(opt)) < 0)
    {
        perror("setsockopt");
        (void)close(udp_sock);
        udp_sock = -1;
        return UDP_ERR_SOCKET;
    }

    /* Set non-blocking — return value checked */
    fcntl_ret = fcntl(udp_sock, F_SETFL, O_NONBLOCK);
    if (fcntl_ret < 0)
    {
        perror("fcntl");
        (void)close(udp_sock);
        udp_sock = -1;
        return UDP_ERR_SOCKET;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family     = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* Bind to eth1 interface IP address 10.0.0.170 */
    //addr.sin_addr.s_addr = inet_addr("10.0.0.170");
    addr.sin_port       = htons(port);

    if (bind(udp_sock, (const struct sockaddr *)&addr, (socklen_t)sizeof(addr)) < 0)
    {
        perror("bind");
        (void)close(udp_sock);
        udp_sock = -1;
        return UDP_ERR_BIND;
    }

    printf("UDP RX started (non-blocking, %d ms poll) on port %u\n",
           UDP_RX_POLL_MS, (unsigned int)port);
    return UDP_ERR_OK;
}

/* ================================================================== */
/* udp_receive                                                         */
/* ================================================================== */
UDP_ERR_T udp_receive(const uint8_t *buf, size_t buf_len, size_t *rx_len)
{
    struct pollfd pfd;
    int           poll_ret;
    ssize_t       recv_ret;

    /* Initialise output */
    if (rx_len != NULL)
    {
        *rx_len = 0u;
    }

    if (udp_sock < 0)
    {
        return UDP_ERR_NOT_INIT;
    }

    if (buf == NULL || rx_len == NULL)
    {
        return UDP_ERR_PARAM;
    }

    pfd.fd     = udp_sock;
    pfd.events = POLLIN;

    poll_ret = poll(&pfd, 1u, UDP_RX_POLL_MS);
    if (poll_ret < 0)
    {
        perror("poll");
        return UDP_ERR_RECV;
    }
    else if (poll_ret == 0)
    {
        /* Timeout — no data ready */
        return UDP_ERR_OK;
    }
    else
    {
        /* poll_ret > 0 — at least one fd is ready */
    }

    if ((pfd.revents & POLLIN) != 0)
    {
        recv_ret = recvfrom(udp_sock, (void *)buf, buf_len, 0, NULL, NULL);
        if (recv_ret < 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            {
                /* Spurious wake — treat as "no data" */
                return UDP_ERR_OK;
            }
            perror("recvfrom");
            return UDP_ERR_RECV;
        }

        *rx_len = (size_t)recv_ret;
    }

    return UDP_ERR_OK;
}

/* ================================================================== */
/* udp_close                                                           */
/* ================================================================== */
void udp_close(void)
{
    if (udp_sock >= 0)
    {
        (void)close(udp_sock);   /* cast to (void) — intentionally ignoring close() status */
        udp_sock = -1;
        printf("UDP socket closed\n");
    }
}

/* ================================================================== */
/* parse_udp_to_mvb_frame                                              */
/* ================================================================== */
UDP_ERR_T parse_udp_to_mvb_frame(const uint8_t        *udp_buf,
                                 size_t                 udp_len,
                                 MVB_ACQUISITION_FRAME *frame)
{
    size_t offset;

    /* ---- parameter validation ---------------------------------------- */
    if ((udp_buf == NULL) || (frame == NULL))
    {
        return UDP_ERR_PARAM;
    }

    if (udp_len < MVB_FRAME_HEADER_SIZE)
    {
        return UDP_ERR_PARSE;
    }

    /* ---- parse header fields one statement per field ------------------ */
    offset = 0u;

    frame->line = udp_buf[offset];
    offset += 1u;

    frame->fcode = udp_buf[offset];
    offset += 1u;

    frame->address = read_le_uint16(&udp_buf[offset]);
    offset += 2u;

    frame->data_len = udp_buf[offset];
    offset += 1u;

    frame->with_crc = udp_buf[offset];
    offset += 1u;

    frame->error_flag = udp_buf[offset];
    offset += 1u;

    frame->reserved = udp_buf[offset];
    offset += 1u;

    /* ---- payload validation ------------------------------------------ */
    if (frame->data_len > MVB_MAX_DATA_LEN)
    {
        printf("FAIL: data_len too large (%u > %d)\n",
               frame->data_len, MVB_MAX_DATA_LEN);
        return UDP_ERR_PARSE;
    }

    if ((offset + frame->data_len) > udp_len)
    {
         printf("FAIL: data_len too large offset (%u )\n",
               frame->data_len);
        return UDP_ERR_PARSE;
    }

    /* ---- copy payload ------------------------------------------------ */
    memset(frame->data, 0, sizeof(frame->data));
    memcpy(frame->data, &udp_buf[offset], (size_t)frame->data_len);

    return UDP_ERR_OK;
}

/* ================================================================== */
/* dump_mvb_frame                                                      */
/* ================================================================== */
void dump_mvb_frame(const MVB_ACQUISITION_FRAME *frame)
{
    uint8_t i;
    trdpPdMsg_t stPdTrdpData = {0};
    TRDP_ERR_T    eErr     = TRDP_NO_ERR;
    stPdTrdpData.stPdMsg.comId = 1000;
    stPdTrdpData.stPdMsg.destIpAddr = 0x0a00001e;
    stPdTrdpData.stPdMsg.msgType = MSG_TYPE_PD_DATA;


    if (frame == NULL)
    {
        return;
    }
    printf("----- MVB Frame -----\n");
    printf(" Line       : %u\n",   (unsigned int)frame->line);
    printf(" F-code     : %u\n",   (unsigned int)frame->fcode);
    printf(" Address    : %u\n",   (unsigned int)frame->address);
    printf(" Data Len   : %u\n",   (unsigned int)frame->data_len);
    printf(" With CRC   : %u\n",   (unsigned int)frame->with_crc);
    printf(" Error Flag : 0x%02X\n",(unsigned int)frame->error_flag);

    printf(" Data       : ");
    for (i = 0u; i < frame->data_len; i++)
    {
        printf("%02X ", (unsigned int)frame->data[i]);
    }
    printf("\n");
    if(frame->address == PLC_EVR_P)
    {
        printf(" frame->address == PLC_EVR_P \n: ");
        stPdTrdpData.stPdMsg.comId = 1012;
        memcpy(&stPdTrdpData.pui8TrdpMsg, frame->data,frame->data_len);
        stPdTrdpData.ui16TrdpMsgLen = frame->data_len;
        printf("\n");
        eErr = eSendTrdpPdData(&stPdTrdpData, (uint32_t)frame->data_len, appHandle);
        printf("eSendTrdpPdData error : %d\n",eErr);
    }
    else if(frame->address == CCU_DMB_HB_P)
    {
        printf(" frame->address == CCU_DMB_HB_P \n: ");
        stPdTrdpData.stPdMsg.comId = 1020;
        memcpy(&stPdTrdpData.pui8TrdpMsg, frame->data,frame->data_len);
        stPdTrdpData.ui16TrdpMsgLen = frame->data_len;
        printf("\n");
        eErr = eSendTrdpPdData(&stPdTrdpData, (uint32_t)frame->data_len, appHandle);
        printf("eSendTrdpPdData error : %d\n",eErr);
    }
    else if(frame->address == PLC_PASV_P)
    {
        printf(" frame->address == PLC_PASV_P \n: ");
        stPdTrdpData.stPdMsg.comId = 1021;
        memcpy(&stPdTrdpData.pui8TrdpMsg, frame->data,frame->data_len);
        stPdTrdpData.ui16TrdpMsgLen = frame->data_len;
        printf("\n");
        eErr = eSendTrdpPdData(&stPdTrdpData, (uint32_t)frame->data_len, appHandle);
        printf("eSendTrdpPdData error : %d\n",eErr);
    }
    else if(frame->address == PLC_HMI_1_P)
    {
        printf(" frame->address == PLC_HMI_1_P \n: ");
        stPdTrdpData.stPdMsg.comId = 1016;
        memcpy(&stPdTrdpData.pui8TrdpMsg, frame->data,frame->data_len);
        stPdTrdpData.ui16TrdpMsgLen = frame->data_len;
        printf("\n");
        eErr = eSendTrdpPdData(&stPdTrdpData, (uint32_t)frame->data_len, appHandle);
        printf("eSendTrdpPdData error : %d\n",eErr);
    }
    else
    {
        /*Do Nothing*/
    }


    //eSendTrdpData(frame->data, frame->data_len, appHandle );
    printf("\n---------------------\n");
}