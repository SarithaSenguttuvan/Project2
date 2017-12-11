/*
 * gemericTiva.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "genericTiva.h"

//Queue handles
extern QueueHandle_t xMainQueue;

void sendHB(uint8_t taskID, tiva_msgStruct_t *tivaMsg)
{
    BaseType_t xGenericStatus;
    //char xGenericOpMsg[100];
   // tiva_msgStruct_t socketHBMsg;
    UARTprintf("\r\n generic():: Sending HB ");
    tivaMsg->tivaMsgId = TIVA_MSGID_HB_RESP;
    tivaMsg->tivaMsgSrcTask = taskID;
    tivaMsg->tivaMsgPayload = NULL;
    tivaMsg->tivaMsgPayloadLen = 0;

    xGenericStatus = xQueueSendToBack( xMainQueue, (void *)tivaMsg, 0 );
    UARTprintf("\r\n<<<<");
    if( xGenericStatus == pdPASS )
    {
        //sprintf(xGenericOpMsg, "HB msg sent from task ID %d\n",taskID);
        UARTprintf("\r\nHB msg >>>>>sent from");
    }
    return;
}
