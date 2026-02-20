/*
 *      Copyright (c) 2025 Bacancy System Pvt Ltd.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *      @author: BACANCY SYSTEMS PVT. LTD. (Developer: Vivek pal) 
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
 
#ifndef GW_MQTT_H
#define GW_MQTT_H
 
/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include "mosquitto.h"



/***********************************************************************
 * CONSTANTS
 **********************************************************************/

#define BROKER "broker.hivemq.com"
#define PORT 1883
#define TOPIC "ccmp13/test"

/***********************************************************************
* TYPES
***********************************************************************/



/***********************************************************************
* PUBLIC VARIABLES 
***********************************************************************/


/***********************************************************************
* PUBLIC FUNCTION PROTOTYPES
***********************************************************************/

/**
 * @brief 
 * 
 * @param arg 
 * @return void* 
 */
void *pvMqttThread(void *arg);

/***********************************************************************
* END OF FILE
***********************************************************************/
#endif
