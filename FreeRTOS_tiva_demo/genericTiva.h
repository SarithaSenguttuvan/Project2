/*
 * genericTiva.h
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#ifndef GENERICTIVA_H_
#define GENERICTIVA_H_

#include <stdint.h>
#include <stdbool.h>
#include "drivers/pinout.h"
#include "utils/uartstdio.h"

// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

// FreeRTOS includes
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TIVA_MAIN_TASK_ID 6
#define TIVA_SOCKET_TASK_ID 7
#define TIVA_LIGHT_TASK_ID 8
#define TIVA_ACCELEROMETER_TASK_ID 9

/* Enum for type of message i.e., Message ID on TIVA */
typedef enum
{
    TIVA_MSGID_HB_RESP,
    TIVA_MSGID_HB_REQ,
    TIVA_MSGID_LIGHT_REQ,
    TIVA_MSGID_LIGHT_DATA,
    TIVA_MSGID_ACCELEROMETER_REQ,
    TIVA_MSGID_ACCELEROMETER_DATA,
    TIVA_MSGID_LIGHT_AUTO,
    TIVA_MSGID_LIGHT_CTRL,
    TIVA_MSID_ALERT,
    TIVA_MSGID_ERROR,
    TIVA_MSGID_LOGMSG
}tiva_msgid_t;

/* Structure for a message ON TIVA*/
typedef struct
{
    tiva_msgid_t tivaMsgId;
    uint8_t tivaMsgSrcTask;             /* Source Task ID */
    size_t tivaMsgPayloadLen;           /* Msg length  */
    void * tivaMsgPayload;              /* Msg associated data */
}tiva_msgStruct_t;


/**
 * @brief
 *      This function is used to send heartbeat messages to the main queue
 * @param
 *      tivaSrcTaskId - Holds the sourcetask ID that sends the Heartbeats
 *      logPacket - holds the log packet of type log_t
 * @return
 *      void
 */
void sendHB(uint8_t taskID, tiva_msgStruct_t *tivaMsg);
#endif /* GENERICTIVA_H_ */
