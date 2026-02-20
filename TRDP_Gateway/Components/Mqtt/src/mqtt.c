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
#include "mqtt.h"
#include "GW_common.h"
#include "Udp_Rx.h"
#include "GW_trdp_pd.h"

/************************************************************************
 * Define Macros
************************************************************************/
#define READ_BIT_16(value, pos)   (((value) >> (pos)) & 0x1U)
#define COM_ADDRESS               (PLC_HMI_1_P + 1000U )

/************************************************************************
 * Define Enumeration/Structure/Unions
************************************************************************/
typedef enum
{
    WM = 0,
    LSRM,
    HSRM,
    RM,
    ATP,
    ATO,
    DMATCU1CUT,
    DMATCU2CUT,
    MCATCU1CUT,
    MCATCU2CUT,
    MCBTCU1CUT,
    MCBTCU2CUT,
    DMBTCU1CUT,
    DMBTCU2CUT
} PLC_HMI_ControlW_e;


/************************************************************************
 * Define Private Variables
************************************************************************/


/************************************************************************
 * Define Global Variables
************************************************************************/


/**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
**********************************************************************/


/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/


/***********************************************************************
 * PUBLIC FUNCTIONS

 **********************************************************************/

 /**
  * @brief 
  * 
  * @param arg 
  * @return void* 
  */
void *pvMqttThread(void *arg)
{
    struct mosquitto *mosq;
    int rc = 0;
    uint8_t *PlcHmi1_P = NULL;
    char payload[2000] = {0};
    int ErrMqtt = 0;

    printf("this is debug print 1\n");
   // mosquitto_lib_init();

    printf("Connect failed: %d\n",  mosquitto_lib_init());
    mosq = mosquitto_new("ccmp13_client", true, NULL);
    if (!mosq)
    {
        printf("Failed to create client\n");
        return -1;
    }
    rc = mosquitto_connect(mosq, BROKER, PORT, 60);
    if (rc != MOSQ_ERR_SUCCESS)
    {
        printf("Connect failed: %s\n", mosquitto_strerror(rc));
        return -1;
    }

    mosquitto_loop_start(mosq);

    while (1)
    {
        PlcHmi1_P = pvGetDataFromDb(COM_ADDRESS);
        if (PlcHmi1_P == NULL) 
        {
            printf("the buffer is empty \n");
        }
        printf("this is debug print\n");
        snprintf(payload, sizeof(payload), "PLC_BattVolt_MCA: %d\n", ((PlcHmi1_P[8] << 8)  | (PlcHmi1_P[9])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BattVolt_MCB: %d\n",    ((PlcHmi1_P[10] << 8) | (PlcHmi1_P[11])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_DMA:   %0.2f\n", (float)((PlcHmi1_P[12] << 8) | (PlcHmi1_P[13])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_DMB:   %0.2f\n", (float)((PlcHmi1_P[14] << 8) | (PlcHmi1_P[15])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_MCA:   %0.2f\n", (float)((PlcHmi1_P[16] << 8) | (PlcHmi1_P[17])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_MCB:   %0.2f\n", (float)((PlcHmi1_P[18] << 8) | (PlcHmi1_P[19])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_TCA:   %0.2f\n", (float)((PlcHmi1_P[20] << 8) | (PlcHmi1_P[21])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_BraPre_TCB:   %0.2f\n", (float)((PlcHmi1_P[22] << 8) | (PlcHmi1_P[23])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_LineVolt_TCA: %d\n", ((PlcHmi1_P[24] << 8) | (PlcHmi1_P[25])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_LineVolt_TCB: %d\n", ((PlcHmi1_P[26] << 8) | (PlcHmi1_P[27])));
        snprintf(&payload[strlen(payload)], sizeof(payload), "PLC_HMI_ControlW: %d\n", ((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])));

        snprintf(&payload[strlen(payload)], sizeof(payload), "WM :         %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), WM));
        snprintf(&payload[strlen(payload)], sizeof(payload), "LSRM :       %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), LSRM));
        snprintf(&payload[strlen(payload)], sizeof(payload), "HSRM :       %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), HSRM));
        snprintf(&payload[strlen(payload)], sizeof(payload), "RM :         %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), RM));
        snprintf(&payload[strlen(payload)], sizeof(payload), "ATP :        %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), ATP));
        snprintf(&payload[strlen(payload)], sizeof(payload), "ATO :        %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), ATO));
        snprintf(&payload[strlen(payload)], sizeof(payload), "DMATCU1Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), DMATCU1CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "DMATCU2Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), DMATCU2CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "MCATCU1Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), MCATCU1CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "MCATCU2Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), MCATCU2CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "MCBTCU1Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), MCBTCU1CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "MCBTCU2Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), MCBTCU2CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "DMBTCU1Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), DMBTCU1CUT));
        snprintf(&payload[strlen(payload)], sizeof(payload), "DMBTCU2Cut : %d\n", READ_BIT_16(((PlcHmi1_P[28] << 8) | (PlcHmi1_P[29])), DMBTCU2CUT));

       ErrMqtt =  mosquitto_publish(mosq,
                          NULL,
                          TOPIC,
                          strlen(payload),
                          payload,
                          1,
                          false);

        printf("Message published status %d\n",ErrMqtt );
        sleep(1);
    }

    mosquitto_loop_stop(mosq, true);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;

}

/***********************************************************************
 * END OF FILE
 **********************************************************************/