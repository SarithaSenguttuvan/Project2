/*
 * lightTiva.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "lightTiva.h"
#include "genericTiva.h"

//Queue handles
extern QueueHandle_t xMainQueue;
extern QueueHandle_t xSocketQueue;
extern QueueHandle_t xLightQueue;
extern QueueHandle_t xAccelerometerQueue;

// Function for Light task
void lightTask(void *pvParameters)
{
    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, 0) != 0)
        {
            UARTprintf("\r\n Light():: Received HB request");
            //*TBD* send HB
        }
    }
}


