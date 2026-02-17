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

#include <stdlib.h>
#include <unistd.h>


/**
 * @brief  Application entry point.
 * @return EXIT_SUCCESS on normal shutdown, EXIT_FAILURE on init error.
 */
int main(void)
{
    /* ---- main receive loop ------------------------------------------ */
    printf("TRDP GATEWAY FW VERSION %d.%d\r\n",FW_VERSION_MAJOR,FW_VERSION_MINOR);
    vUserLedInit();
    iUartTtySTM2Type = iUartInit(UART_TTYSTM2_PORT, UART_TTYSTM2_BAUDRATE);

    pthread_t stThreadTrdpId, stuartRecId, stUdpThreadId;
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
        //iRet = pthread_create(&stuartRecId, NULL, pvUartRxThread, NULL);
        iRet = pthread_create(&stUdpThreadId, NULL, pvUdpThread, NULL);
        if (iRet != 0)
        {
            vos_printLog(VOS_LOG_ERROR," UART Thread Init failed!\n");
        }
        else
        {
            while (TRUE)
            {
                if(iUledToggle() == 1)
                {
                    printf(" led toggle error\r\n");
                }
                else
                {
                    /*Do Nothing*/
                }
                /* Yield to reduce CPU consumption */
                (void)sleep(1);
            }
        }
    }
    return EXIT_SUCCESS;
}