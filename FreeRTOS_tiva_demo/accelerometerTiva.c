/*
 * accelerometer.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "accelerometerTiva.h"
#include "genericTiva.h"

//Queue handles
extern QueueHandle_t xMainQueue;
extern QueueHandle_t xSocketQueue;
extern QueueHandle_t xLightQueue;
extern QueueHandle_t xAccelerometerQueue;

#define ACCELEROMETER_TASK_WAIT_TIME   pdMS_TO_TICKS( 2000 )

// Function for accelerometer task
void accelerometerTask(void *pvParameters)
{
    tiva_msgStruct_t accelerometerHBMsg;
    UARTprintf("\r\n In the accelerometer task");
    for (;;)
    {
        if(ulTaskNotifyTake( pdTRUE, ACCELEROMETER_TASK_WAIT_TIME) != 0)
        {
            sendHB(TIVA_ACCELEROMETER_TASK_ID, &accelerometerHBMsg);
            UARTprintf("\r\n Accelerometer():: ?????Received HB request");
            //*TBD* send HB
        }
    }
}


