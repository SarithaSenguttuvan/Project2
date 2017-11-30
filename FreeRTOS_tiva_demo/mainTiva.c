/*
 * mainTask.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */



#include "mainTiva.h"
#include "genericTiva.h"
#include <timers.h>

#define mainONE_SHOT_TIMER_PERIOD pdMS_TO_TICKS( 3333 )
#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 2000 )

//Queue handles
QueueHandle_t xMainQueue;
QueueHandle_t xSocketQueue;
QueueHandle_t xLightQueue;
QueueHandle_t xAccelerometerQueue;

// Function for main task
void mainTask(void *pvParameters)
{
    TimerHandle_t xMyTimer;
    xMyTimer = xTimerCreate("mySWTimer",mainAUTO_RELOAD_TIMER_PERIOD,pdTRUE,0,myTimerCallBack );
    if(xmyTimer == NULL)
        return;
    xMainQueue = xQueueCreate( 30, sizeof( int32_t ) );    /* Create Main Queue */
    if(xMainQueue == NULL)
        return;
    xSocketQueue = xQueueCreate( 30, sizeof( int32_t ) );    /* Create Socket Queue */
    if(xSocketQueue == NULL)
        return;
    xLightQueue = xQueueCreate( 30, sizeof( int32_t ) );    /* Create Light Queue */
    if(xLightQueue == NULL)
        return;
    xAccelerometerQueue = xQueueCreate( 30, sizeof( int32_t ) );    /* Create Accelerometer Queue */
    if(xAccelerometerQueue == NULL)
        return;

    for (;;)
    {

    }
}

/* Timer Call-Back function*/
static void myTimerCallBack( TimerHandle_t xTimer )
{
    //TickType_t xTimeNow;
    //xTimeNow = uxTaskGetTickCount();    /* Obtain the current tick count. */
    /* Output a string to show the time at which the callback was executed. */
    UARTprintf("\r\n Received Timer signal");
    xTaskNotifyGive( xSocketQueue );
    xTaskNotifyGive( xLightQueue );
    xTaskNotifyGive( xAccelerometerQueue );
    /*TBD* send over socket "HeartBeats received
    //ulCallCount++;
}
