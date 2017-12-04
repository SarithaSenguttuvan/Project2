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
///#define SOCKET_HB_REQ_BIT (0x01)
//#define SET_BIT (1)

// Function for socket task
void socketTask(void *pvParameters)
{
    //BaseType_t xSocketStatus;
    tiva_msgStruct_t socketHBMsg;
    uint32_t socketNotificationValue = 0;

    UARTprintf("\r\n In the Socket task");

    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &socketNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            if((socketNotificationValue & HB_REQ_BIT) == SET_BIT)
            {
                sendHB(TIVA_SOCKET_TASK_ID, &socketHBMsg);
                UARTprintf("\r\n Socket():: ?????Received HB request");
                //*TBD* send HB
                //xSocketStatus = xQueueSendToBack( xMainQueue, &socketHBMsg, 0 );
            }
        }
        else
        {
            UARTprintf("\r\n Socket():: HB request receive error");
        }
        //xReturn = ( BaseType_t ) ulTaskNotifyTake( pdTRUE, pxUARTInstance->xTxTimeout );
    }
}


