/*
 * socketTiva.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "socketTiva.h"
#include "genericTiva.h"


//Queue handles
extern QueueHandle_t xMainQueue;
extern QueueHandle_t xSocketQueue;
extern QueueHandle_t xLightQueue;
extern QueueHandle_t xAccelerometerQueue;

#define SOCKET_TASK_WAIT_TIME   pdMS_TO_TICKS( 2000 )
#define LIGHT_SOCKET ("light$")
#define ACC_SOCKET ("accelerometer$")
#define ERR_SOCKET ("error$")

// Function for socket task
void socketTask(void *pvParameters)
{
    //BaseType_t xSocketStatus;
    size_t socketMsgCount = 0;
    tiva_msgStruct_t socketHBMsg;
    BaseType_t xSocketStatus;
    uint32_t socketNotificationValue = 0;
    tiva_msgStruct_t socketReceivedmsg;

    UARTprintf("\r\n In the Socket task");

    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &socketNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            if((socketNotificationValue & HB_REQ_BIT))
            {
                sendHB(TIVA_SOCKET_TASK_ID, &socketHBMsg);
                UARTprintf("\r\n Socket():: ?????Received HB request");
                //*TBD* send HB
                //xSocketStatus = xQueueSendToBack( xMainQueue, &socketHBMsg, 0 );
            }
            if((socketNotificationValue & MQ_RECEIVE_BIT))
            {
                while((socketMsgCount = uxQueueMessagesWaiting(xSocketQueue)) != 0)
                {
                    xSocketStatus = xQueueReceive( xSocketQueue, (tiva_msgStruct_t *)&socketReceivedmsg, 0 );
                    if( xSocketStatus == pdPASS )
                    {
                        //*TBD* send logs
                        UARTprintf("\r\n socket():: xQueueReceive Success");
                    }
                    else
                    {
                        //*TBD* send logs
                        UARTprintf("\r\n Socket():: xQueueReceive Failure");
                    }
                    if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_LIGHT_DATA)
                    {
                        //Send to socket ('A') send(LIGHT_SOCKET);
                    }
                    else if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_ACCELEROMETER_DATA)
                    {
                        //Send to socket ('B') send(ACC_SOCKET);
                    }
                    else if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_ERROR)
                    {
                        //Send to socket ('C') send(ERR_SOCKET);
                    }
                }
                UARTprintf("\r\n Socket():: ?????Received HB request");
            }
        }
        else
        {
            UARTprintf("\r\n Socket()::xTaskNotifyWait error");
        }
        //xReturn = ( BaseType_t ) ulTaskNotifyTake( pdTRUE, pxUARTInstance->xTxTimeout );
    }
}


