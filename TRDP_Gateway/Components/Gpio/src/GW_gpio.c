/*
 *      Copyright (c) 2024 Bacancy System Pvt Ltd.
 *      All rights reserved.
 *      Use of copyright notice does not imply publication.
 *      @author: BACANCY SYSTEMS PVT. LTD. (Developer: VRUSHAL VARA) 
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
/*!
 * \file
 * \brief
 */

/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include "GW_gpio.h"
 
/***********************************************************************
 * CONSTANTS
 **********************************************************************/
static gpio_t *stUserLedOutput = {0};
static gpio_value_t stUserLedState = GPIO_LOW;
 
 /**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 **********************************************************************/
 
/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/

/***********************************************************************
 * PUBLIC FUNCTIONS
 **********************************************************************/
void vUserLedInit(void)
{
	stUserLedOutput = ldx_gpio_request_by_controller("gpiochip6", 0, GPIO_OUTPUT_LOW);
    if(!stUserLedOutput)
    {
		printf("Failed to initialize output User LED\n"); 
    }
    else
    {
        /*Do Nothing*/
    }
}

int iUledToggle(void)
{
    int RetError = 0;
    stUserLedState = (stUserLedState == GPIO_LOW) ? GPIO_HIGH : GPIO_LOW;
    if(stUserLedOutput)
    {
        ldx_gpio_set_value(stUserLedOutput, stUserLedState);
    }
    else
    {
        RetError = 1;
    }
    return RetError;
}

 

 
/***********************************************************************
 * END OF FILE
 **********************************************************************/