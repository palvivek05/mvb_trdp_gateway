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

/***********************************************************************
 * Include Header Files
 **********************************************************************/
#include "GW_uart.h"
#include "GW_trdp_pd.h"
#include "GW_trdp_app.h"
 
/***********************************************************************
 * CONSTANTS
 **********************************************************************/
int iUartTtySTM2Type = 0;
UartRecBuff_t stUartLiveRecData = {0};
 
 /**********************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 **********************************************************************/
static uint16_t u16CalculateCrc(uint8_t * pu8Data, uint16_t u16DataLen);
static int      iPherUartInit(const char *dev , speed_t uibaudRate);
static int      iPherUartSend(int iUartInitType, uint8_t * pu8DataBuff, uint32_t u32DataSize);
static int      iPherUartRec(int iUartInitType, uint8_t * pu8DataBuff, uint32_t u32DataSize);
static void     vUartRxParser(int iUartInitType, uint8_t *pu8Data, uint32_t u32Datasize);
 
/***********************************************************************
 * PRIVATE FUNCTIONS
 **********************************************************************/
/**
 * @brief: CRC Calculation Driver API
 * 
 * @param[in]  u16DataLen : Data Len Of Buffer
 * @param[in]  pu8Data    : Data Pointer
 * @param[out] u16Crc     : Calculated CRC
 */
static uint16_t u16CalculateCrc(uint8_t * pu8Data, uint16_t u16DataLen)
{
    uint16_t u16Index = 0U;
    uint16_t u16Crc = 0U;

    for(u16Index = 0; u16Index < u16DataLen; u16Index++)
    {
        u16Crc += pu8Data[u16Index];
    }
    return u16Crc;
}

static int iPherUartInit(const char *dev , speed_t uibaudRate)
{
    struct termios tty;
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0)
    {
        perror("open");
        fd = -1;
    }
    else
    {
        memset(&tty, 0, sizeof tty);

        if (tcgetattr(fd, &tty) != 0) 
        {
            perror("tcgetattr");
            close(fd);
            fd = -1;
        }
        else
        {
            cfsetospeed(&tty, uibaudRate);
            cfsetispeed(&tty, uibaudRate);

            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
            tty.c_cflag &= ~PARENB;   // no parity
            tty.c_cflag &= ~CSTOPB;   // 1 stop bit
            tty.c_cflag &= ~CRTSCTS;  // no hw flow ctrl
            tty.c_cflag |= CLOCAL | CREAD;

            tty.c_iflag = 0;  // Clear all input flags
            tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // shut off xon/xoff ctrl
            tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); 

            // Local flags (c_lflag) - RAW MODE
            tty.c_lflag = 0;  // Clear all local flags - RAW INPUT
            tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG | IEXTEN);

            // Output flags (c_oflag) - RAW OUTPUT
            tty.c_oflag = 0;  // Clear all output flags - RAW OUTPUT
            tty.c_oflag &= ~OPOST;

            tty.c_cc[VMIN]  = 1;
            tty.c_cc[VTIME] = 0;

            tcflush(fd, TCIOFLUSH);

            if (tcsetattr(fd, TCSANOW, &tty) != 0)
            {
                perror("tcsetattr");
                close(fd);
                fd = -1;
            }
        }
    }
    return fd;
}

static int iPherUartSend(int iUartInitType, uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    int iRet = 0;
    iRet = write(iUartInitType, pu8DataBuff, u32DataSize);
    return iRet;
}

static int iPherUartRec(int iUartInitType, uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    int iRet = 0;
    iRet = read(iUartInitType, pu8DataBuff, u32DataSize);
    return iRet;
}

static void vUartRxParser(int iUartInitType, uint8_t *pu8Data, uint32_t u32Datasize)
{
    if(TRUE == stUartLiveRecData.u8DataReady)
    {
        /*Do Nothing*/
    }
    else
    {
        if(0U == stUartLiveRecData.u32ReadBytesSize)
        {
            if(SOF_BYTE_VAL == pu8Data[SOF_BYTE_POS])
            {
                memcpy(stUartLiveRecData.au8Data, pu8Data, u32Datasize);
                stUartLiveRecData.u32ReadBytesSize = u32Datasize;
            }
            else
            {
                /*Do Nothing*/
            }
        }
        else
        {
            memcpy(&stUartLiveRecData.au8Data[stUartLiveRecData.u32ReadBytesSize], pu8Data, u32Datasize);
            stUartLiveRecData.u32ReadBytesSize += u32Datasize;

            uint32_t u32SizeofFream = 0U;
            memcpy(&u32SizeofFream, &stUartLiveRecData.au8Data[LEN_LSB_BYTE_POS], sizeof(uint32_t));

            if((stUartLiveRecData.u32ReadBytesSize) >= (u32SizeofFream + 8U))
            {
                stUartLiveRecData.u8DataReady = TRUE;
            }
            else
            {
                /*Do Nothing*/
            }
        }
    }
}
/***********************************************************************
 * PUBLIC FUNCTIONS
 **********************************************************************/
 
 
/**
 * @brief:
 *
 * @param[in]
 * @param[out]
 */
int iUartInit(char *pcPort, int iBoardRate)
{
    int iRet = 0;
    iRet = iPherUartInit(pcPort, iBoardRate);
    return iRet;
}

int iUartSend(int iUartType, uint16_t u16CommandId, uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    int iRet = 0;
    if(iUartType < 0)
    {
        iRet = -1;
    }
    else
    {
        if((NULL != pu8DataBuff) && (u32DataSize >= 0U))
        {
            uint16_t u16CrcCal = 0x00U;
            uint8_t * pu8MemoryBuff = (uint8_t *)malloc(u32DataSize + EXTRA_BYTES_LEN);
            if(NULL != pu8MemoryBuff)
            {
                /*SOF*/
                pu8MemoryBuff[SOF_BYTE_POS]         = SOF_BYTE_VAL;

                /*DATA LEN*/
                uint32_t u32DataLen = u32DataSize + 2U;
                memcpy(&pu8MemoryBuff[LEN_LSB_BYTE_POS], &u32DataLen, sizeof(uint32_t));

                /*COMMAND ID*/
                pu8MemoryBuff[CMDID_LSB_BYTE_POS]   = ((u16CommandId) & 0xFF);
                pu8MemoryBuff[CMDID_MSB_BYTE_POS]   = (((u16CommandId) >> 8U) & 0xFF);

                /*DATA*/
                memcpy(&pu8MemoryBuff[DATA_START_BYTE_POS], pu8DataBuff, u32DataSize);

                /*CRC*/
                u16CrcCal = u16CalculateCrc(&pu8MemoryBuff[CMDID_LSB_BYTE_POS], u32DataSize + 2U);
                memcpy(&pu8MemoryBuff[DATA_START_BYTE_POS + u32DataSize], &u16CrcCal, sizeof(u16CrcCal));

                /*EOF*/
                pu8MemoryBuff[DATA_START_BYTE_POS + u32DataSize + sizeof(uint16_t)] = EOF_BYTE_VAL;

                if((u32DataSize + EXTRA_BYTES_LEN) != iPherUartSend(iUartType, pu8MemoryBuff, (u32DataSize + EXTRA_BYTES_LEN)))
                {
                    iRet = -1; 
                }
                else
                {
                    /*Do Nothing*/
                }
                free(pu8MemoryBuff);
            }
            else
            {
                iRet = -1; 
            }

        }
        else
        {
            iRet = -1;
        }
    }
    return iRet;
    
}

int iUartRec(int iUartType, uint8_t * pu8DataBuff, uint32_t u32DataSize)
{
    int iRet = 0;
    if(iUartType < 0)
    {
        iRet = -1;
    }
    else
    {
        if(NULL != pu8DataBuff)
        {
            int iDataRecBytes = iPherUartRec(iUartType, pu8DataBuff, u32DataSize);
            if(0 < iDataRecBytes)
            {
                vUartRxParser(iUartType, pu8DataBuff, iDataRecBytes);
            }
        }
        else
        {
            iRet = -1;
        }
    }
    if(stUartLiveRecData.u8DataReady == TRUE)
    {
        printf("Data Ready >>>> size of data %d\r\n", stUartLiveRecData.u32ReadBytesSize);
        eSendTrdpData(stUartLiveRecData.au8Data, stUartLiveRecData.u32ReadBytesSize, appHandle );
        stUartLiveRecData.u8DataReady = FALSE;
        memset(stUartLiveRecData.au8Data, 0x00, sizeof(stUartLiveRecData.u32ReadBytesSize));
        stUartLiveRecData.u32ReadBytesSize = 0x00;
    }
    else
    {
        /*Do Nothing*/
    }
    return iRet;
}


void *pvUartRxThread(void *arg)
{
    uint8_t u8Data = 0;
    while(1)
    {
        iUartRec(iUartTtySTM2Type, &u8Data, 1U);
        // usleep(1);
    }
}
/***********************************************************************
 * END OF FILE
 **********************************************************************/