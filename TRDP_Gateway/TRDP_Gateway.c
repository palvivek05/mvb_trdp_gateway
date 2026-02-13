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
 * \file    main_v4.c
 * \brief   Application entry point — UDP receive loop.
 */

/****/
/* MISRA-C 2010 Compliance Notes                                       */
/*                                                                     */
/* R2.2  — Unused macro UART_SEND removed.                             */
/* R5.1  — Unused globals mvb_frame and retp removed.                  */
/* R8.1  — All declarations moved to the top of their block.          */
/*          main() returns EXIT_SUCCESS / EXIT_FAILURE, not raw ints.  */
/* R10.1 — signal() return values checked.                             */
/* R12.1 — keep_running compared explicitly with != 0.                 */
/* R14.3 — Every if/else branch enclosed in braces.                    */
/* R17.1 — Large buffer declared static to keep it off the stack.      */
/* R17.8 — Copy-paste printf bug removed.                              */
/****/

/****/
/* Include Header Files                                                */
/****/
#include "GW_common.h"
#include "GW_gpio.h"
#include "GW_trdp_pd.h"
#include "GW_uart.h"
#include "Udp_Rx.h"

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/****/
/* FILE-SCOPE CONSTANTS                                                */
/****/
/* (none required — port and buffer size come from Udp_Rx.h)           */

/****/
/* FILE-SCOPE STATE                                                    */
/****/
static volatile sig_atomic_t keep_running = 1;

/* Large receive buffer placed in static storage to avoid stack usage  */
static uint8_t g_rx_buffer[UDP_RX_BUFFER_SIZE];

/****/
/* PRIVATE FUNCTION PROTOTYPES                                         */
/****/
static void handle_signal(int sig);
static void print_raw_hex(const uint8_t *buf, size_t len);

/****/
/* PRIVATE FUNCTIONS                                                   */
/****/

/**
 * @brief  SIGINT / SIGTERM handler — sets the run flag to zero.
 * @param  sig  Signal number (unused; cast to void for MISRA).
 */
static void handle_signal(int sig)
{
    (void)sig;
    keep_running = 0;
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

/****/
/* PUBLIC FUNCTIONS                                                    */
/****/

/**
 * @brief  Application entry point.
 * @return EXIT_SUCCESS on normal shutdown, EXIT_FAILURE on init error.
 */
int main(void)
{
    /* All declarations at the top of the block */
    MVB_ACQUISITION_FRAME frame;
    size_t                rx_len;
    UDP_ERR_T             init_ret;
    UDP_ERR_T             recv_ret;
    UDP_ERR_T             parse_ret;
    sighandler_t          prev_sigint;
    sighandler_t          prev_sigterm;

    printf("/\\Ganpti Bapa Moriya/\\\r\n");

    /* Install signal handlers — check previous handler for validity */
    prev_sigint  = signal(SIGINT,  handle_signal);
    prev_sigterm = signal(SIGTERM, handle_signal);
    if ((prev_sigint == SIG_ERR) || (prev_sigterm == SIG_ERR))
    {
        printf("Failed to install signal handlers\n");
        return EXIT_FAILURE;
    }

    /* Initialise UDP receiver */
    init_ret = udp_init(UDP_PORT);
    if (init_ret != UDP_ERR_OK)
    {
        printf("UDP RX init failed (error %d)\n", (int)init_ret);
        return EXIT_FAILURE;
    }

    /* ---- main receive loop ------------------------------------------ */
    printf("TRDP GATEWAY FW VERSION %d.%d\r\n",FW_VERSION_MAJOR,FW_VERSION_MINOR);
    vUserLedInit();
    iUartTtySTM2Type = iUartInit(UART_TTYSTM2_PORT, UART_TTYSTM2_BAUDRATE);

    pthread_t stThreadTrdpId, stuartRecId;
    int iRet = 0;

    /**
     * @brief 
     * 
     */
    iRet = pthread_create(&stThreadTrdpId, NULL, pvEthTrdp, NULL);
    if (iRet != 0)
    {
        vos_printLog(VOS_LOG_ERROR," Ethernet Task creation failed!\n");
    }
    else
    {
        iRet = pthread_create(&stuartRecId, NULL, pvUartRxThread, NULL);
        if (iRet != 0)
        {
            vos_printLog(VOS_LOG_ERROR," UART Thread Init failed!\n");
        }
        else
        {
            while (keep_running != 0)
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
                if(iUledToggle() == 1)
                {
                    printf(" led toggle error\r\n");
                }
                else
                {
                    /*Do Nothing*/
                }
                /* Yield to reduce CPU consumption */
                (void)usleep(10u);
            }
        }
    }
    printf("UDP RX stopping...\n");
    udp_close();

    return EXIT_SUCCESS;
}