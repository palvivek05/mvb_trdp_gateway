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

/****/
/* FILE-SCOPE CONSTANTS                                                */
/****/
/* (none required — port and buffer size come from Udp_Rx.h)           */

/****/
/* FILE-SCOPE STATE                                                    */
/****/


/* Large receive buffer placed in static storage to avoid stack usage  */
static uint8_t g_rx_buffer[UDP_RX_BUFFER_SIZE];

/****/
/* PRIVATE FUNCTION PROTOTYPES                                         */
/****/

static void print_raw_hex(const uint8_t *buf, size_t len);
/* ------------------------------------------------------------------ */
/* mvb_tx_create_nb_socket()                                           */
/* Creates UDP socket, sets O_NONBLOCK, binds to source IP.           */
/* Returns socket fd on success, -1 on failure.                       */
/* ------------------------------------------------------------------ */
static int mvb_tx_create_nb_socket(void);


int sock = 0;
struct sockaddr_in dst;

/****/
/* PRIVATE FUNCTIONS                                                   */
/****/

static uint16_t read_le_uint16(const uint8_t *p)
{
    uint16_t value;
    value = (uint16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8u));
    return value;
}



/**
 * @brief  Print a byte buffer as hex to stdout.
 * @param  buf  Pointer to data.
 * @param  len  Number of bytes.
 */
static void print_raw_hex(const uint8_t *buf, size_t len)
{
    size_t i;

    for (i = 0u; i < len; i++)
    {
        printf("%02X ", (unsigned int)buf[i]);
    }
    printf("\n");
}

/* ------------------------------------------------------------------ */
/* mvb_tx_create_nb_socket()                                           */
/* Creates UDP socket, sets O_NONBLOCK, binds to source IP.           */
/* Returns socket fd on success, -1 on failure.                       */
/* ------------------------------------------------------------------ */
static int mvb_tx_create_nb_socket(void)
{
    int                sock;
    int                flags;
    int                opt;
    struct sockaddr_in src;

    /* 1. Create UDP socket */
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0)
    {
        perror("[MVB_TX] socket");
        return -1;
    }

    /* 2. Set O_NONBLOCK using fcntl -- return value checked (MISRA R10.1) */
    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0)
    {
        perror("[MVB_TX] fcntl F_GETFL");
        (void)close(sock);
        return -1;
    }

    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        perror("[MVB_TX] fcntl F_SETFL O_NONBLOCK");
        (void)close(sock);
        return -1;
    }

    printf("[MVB_TX] Socket set to O_NONBLOCK\n");

    /* 3. SO_REUSEADDR */
    opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
                   &opt, (socklen_t)sizeof(opt)) < 0)
    {
        perror("[MVB_TX] SO_REUSEADDR");
        /* non-fatal */
    }

    /* 4. Bind to Digi board IP so packet exits via eth1 */
    memset(&src, 0, sizeof(src));
    src.sin_family      = AF_INET;
    src.sin_port        = 0;                          /* OS picks src port */
    src.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(MVB_TX_SRC_IP);   /* 10.0.0.160        */

    if (bind(sock, (struct sockaddr *)&src, (socklen_t)sizeof(src)) < 0)
    {
        // fprintf(stderr, "[MVB_TX] bind %s: %s\n",
        //         MVB_TX_SRC_IP, strerror(errno));
        /* non-fatal: OS will route correctly anyway */
    }
    else
    {
        // printf("[MVB_TX] Bound source to %s\n", MVB_TX_SRC_IP);
    }

    return sock;
}

/**
 * @brief 
 * 
 * @param sock 
 * @param dst 
 * @param cb 
 * @return MVB_TX_ERR_T 
 */
MVB_TX_ERR_T mvb_tx_nonblocking_send(int                        sock,
                                        const struct sockaddr_in  *dst,
                                        const MVB_MASTER_CB       *cb)
{
    ssize_t         sent;
    uint8_t         retry;
    struct timespec retry_sleep;
    MVB_TX_ERR_T    result;

    retry_sleep.tv_sec  = 0;
    retry_sleep.tv_nsec = 1000000L;   /* 1 ms between retries */
    result = MVB_TX_ERR_OK;

    for (retry = 0u; retry < (uint8_t)MVB_TX_SEND_RETRY_MAX; retry++)
    {
        sent = sendto(sock,
                      (const void *)cb,
                      sizeof(MVB_MASTER_CB),
                      0,
                      (const struct sockaddr *)dst,
                      (socklen_t)sizeof(*dst));

        if (sent > 0)
        {
            /* Packet handed to kernel TX buffer -- success */
            printf("[MVB_TX] Sent %zd bytes -> %s:%u  (retry=%u)\n",
                   sent, MVB_TX_DEST_IP, MVB_TX_UDP_PORT, (unsigned int)retry);
            result = MVB_TX_ERR_OK;
            break;
        }

        if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
        {
            /* TX buffer full -- wait 1 ms and retry */
            fprintf(stderr,
                    "[MVB_TX] EAGAIN on retry %u -- TX buffer full\n",
                    (unsigned int)retry);
            (void)nanosleep(&retry_sleep, NULL);
            result = MVB_TX_ERR_WOULDBLOCK;
        }
        else if (errno == EINTR)
        {
            /* Interrupted by signal -- treat as transient, retry */
            result = MVB_TX_ERR_WOULDBLOCK;
        }
        else
        {
            /* Hard error */
            perror("[MVB_TX] sendto");
            result = MVB_TX_ERR_SEND;
            break;
        }
    }

    if (result == MVB_TX_ERR_WOULDBLOCK)
    {
        fprintf(stderr,
                "[MVB_TX] Packet dropped after %u retries (TX buffer busy)\n",
                (unsigned int)MVB_TX_SEND_RETRY_MAX);
    }

    return result;
}


/****/
/* PUBLIC FUNCTIONS                                                    */
/****/


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
        (void)close(udp_sock);
        (void)close(sock);   /* cast to (void) — intentionally ignoring close() status */
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
    TRDP_ERR_T  eErr                = TRDP_NO_ERR;



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

    eSendMvbToTrdp(frame);

    printf("\n---------------------\n");
}


void *pvUdpThread(void *arg)
{
    /* All declarations at the top of the block */
    MVB_ACQUISITION_FRAME frame;
    size_t                rx_len;
    UDP_ERR_T             init_ret;
    UDP_ERR_T             recv_ret;
    UDP_ERR_T             parse_ret;

    uint8_t u8Data = 0;


    /* Initialise UDP receiver */
    init_ret = udp_init(UDP_PORT);
    if (init_ret != UDP_ERR_OK)
    {
        printf("UDP RX init failed (error %d)\n", (int)init_ret);
        return EXIT_FAILURE;
    }

    /* Create non-blocking socket */
    sock = mvb_tx_create_nb_socket();
    if (sock < 0)
    {
        fprintf(stderr, "[MVB_TX] Failed to create non-blocking socket\n");
        return UDP_ERR_SOCKET;
    }

        /* Destination: PC 10.0.0.238:9530 */
    memset(&dst, 0, sizeof(dst));
    dst.sin_family      = AF_INET;
    dst.sin_port        = htons((uint16_t)MVB_TX_UDP_PORT);
    dst.sin_addr.s_addr = inet_addr(MVB_TX_DEST_IP);

    printf("[MVB_TX] Thread started (non-blocking)\n");
    printf("[MVB_TX]   Src     : %s\n",        MVB_TX_SRC_IP);
    printf("[MVB_TX]   Dest    : %s:%u\n",     MVB_TX_DEST_IP, MVB_TX_UDP_PORT);
    printf("[MVB_TX]   Size    : %zu bytes\n", sizeof(MVB_MASTER_CB));
   // printf("[MVB_TX]   Period  : %u ms\n",     MVB_TX_INTERVAL_MS);
    printf("[MVB_TX]   Retries : %u\n",        MVB_TX_SEND_RETRY_MAX);

    while(TRUE)
    {
        rx_len = 0u;

        recv_ret = udp_receive(g_rx_buffer, UDP_RX_BUFFER_SIZE, &rx_len);

        if (recv_ret != UDP_ERR_OK)
        {
            printf("UDP receive error (error %d)\n", (int)recv_ret);
            break;
        }

        if (rx_len > 0u)
        {
            printf("Received %zu bytes\n", rx_len);
            print_raw_hex(g_rx_buffer, rx_len);

            parse_ret = parse_udp_to_mvb_frame(g_rx_buffer, rx_len, &frame);

            if (parse_ret == UDP_ERR_OK)
            {
                dump_mvb_frame(&frame);
            }
            else
            {
                printf("parse_udp_to_mvb_frame failed (error %d)\n", (int)parse_ret);
            }
        }
    }
    printf("UDP RX stopping...\n");
    udp_close();
    return NULL;
}

/**
 * @brief 
 * 
 * @param frame 
 * @return TRDP_ERR_T 
 */
TRDP_ERR_T eSendMvbToTrdp(MVB_ACQUISITION_FRAME *frame)
{
    TRDP_ERR_T eErr = TRDP_NO_ERR;

    if (NULL != frame)
    {
        #define X(TRDP_MSGID, ENABLE, MVBID, COMID, INTERVAL, DEST_IP, SRCIP_1, SRCIP_2, HANDLER_PUB, HANDLER_SUB, MSG_BUFF, MSG_LEN, NEW_DATA, MSG_TYPE )\
        if ((frame->address == MVBID) && (ENABLE == TRUE)) \
        {\
            /*acquisition of the data in internal data base */\
            MSG_LEN = frame->data_len;\
            memcpy(MSG_BUFF, &frame->data, MSG_LEN);\
            eErr = tlp_put(appHandle, HANDLER_PUB, MSG_BUFF, MSG_LEN);\
            if (TRDP_NO_ERR != eErr)\
            {\
                vos_printLog(VOS_LOG_ERROR, "MSG_TYPE_PD_DATA unable to update for PD queue %d \n", eErr);\
            }\
            else\
            {\
                vos_printLog(VOS_LOG_INFO, "MSG_TYPE_PD_DATA Pd data has been recived from uart %d \n", eErr);\
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

MVB_TX_ERR_T mvb_tx_build_buffer(MVB_MASTER_CB *cb,
                                 uint16_t       pd_port,
                                 uint8_t        fcode,
                                 const uint8_t *data,
                                 uint8_t        data_len)
{
    if ((cb == NULL) || (data == NULL))
    {
        return MVB_TX_ERR_PARAM;
    }
    if (data_len > (uint8_t)MMC_DATA_SLOT_LEN)
    {
        return MVB_TX_ERR_PARAM;
    }

    memset(cb, 0, sizeof(MVB_MASTER_CB));

    cb->flag              = (uint16_t)MMC_FLAG_VALUE;
    cb->reserved          = 0u;
    cb->lineA_enable      = 1u;
    cb->lineB_enable      = 1u;
    cb->mvb_master_enable = 1u;
    cb->mmc_work_type     = (uint8_t)MMC_PD_DATA;

    cb->mmc_enable[0] = 1u;
    cb->mmc_addr  [0] = pd_port;
    cb->mmc_arg1  [0] = fcode;
    cb->mmc_arg2  [0] = 1u;           /* source port */

    cb->mmc_item_index[0] = 0u;
    cb->mmc_data_len  [0] = (uint16_t)data_len;
    memcpy(cb->mmc_data[0], data, (size_t)data_len);

    return MVB_TX_ERR_OK;
}

