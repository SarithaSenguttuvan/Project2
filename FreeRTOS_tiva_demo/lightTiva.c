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

#define LIGHT_TASK_WAIT_TIME   pdMS_TO_TICKS( 2000 )

// Function for Light task
void lightTask(void *pvParameters)
{
    UARTprintf("\r\n In the Light task");
    tiva_msgStruct_t lightHBMsg;
    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, LIGHT_TASK_WAIT_TIME) != 0)
        {
            sendHB(TIVA_LIGHT_TASK_ID, &lightHBMsg);
            UARTprintf("\r\n Light():: ?????Received HB request");
            //*TBD* send HB
        }
    }
}


