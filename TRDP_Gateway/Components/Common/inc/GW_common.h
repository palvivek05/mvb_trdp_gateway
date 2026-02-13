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
 * \file
 * \brief
 */
 
#ifndef GW_COMMON_H
#define GW_COMMON_H
 
/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <libdigiapix/gpio.h>
#include <termios.h>
#include <errno.h>
#include "pthread.h"
#include "vos_thread.h"
#include "vos_sock.h"
#include "vos_utils.h"
 
/***********************************************************************
 * CONSTANTS
 **********************************************************************/
#define FW_VERSION_MAJOR          0U
#define FW_VERSION_MINOR          2U

//#define TRUE                      1U
//#define FALSE                     0U
#define PACKED                    __attribute__((packed))


#define UART_TTYSTM2_PORT         "/dev/ttySTM2"
#define UART_TTYSTM2_BAUDRATE     B115200
//#define UART_TTYSTM2_BAUDRATE       B921600
/***********************************************************************
* TYPES
***********************************************************************/

/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/

/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
