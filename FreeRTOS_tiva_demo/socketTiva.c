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
// Function for socket task
void socketTask(void *pvParameters)
{
    //BaseType_t xSocketStatus;
    tiva_msgStruct_t socketHBMsg;

    UARTprintf("\r\n In the Socket task");

    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, SOCKET_TASK_WAIT_TIME) != 0)       //TBD block
        {
            sendHB(TIVA_SOCKET_TASK_ID, &socketHBMsg);
            UARTprintf("\r\n Socket():: ?????Received HB request");
            //*TBD* send HB
            //xSocketStatus = xQueueSendToBack( xMainQueue, &socketHBMsg, 0 );

        }

        //xReturn = ( BaseType_t ) ulTaskNotifyTake( pdTRUE, pxUARTInstance->xTxTimeout );
    }
}


