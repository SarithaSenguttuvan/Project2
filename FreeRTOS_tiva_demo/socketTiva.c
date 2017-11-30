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

// Function for socket task
void socketTask(void *pvParameters)
{
    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, 0) != 0)
        {
            UARTprintf("\r\n Socket():: Received HB request");
            //*TBD* send HB
        }

        //xReturn = ( BaseType_t ) ulTaskNotifyTake( pdTRUE, pxUARTInstance->xTxTimeout );
    }
}


