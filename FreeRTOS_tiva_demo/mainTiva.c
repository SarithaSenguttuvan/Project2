/*
 * mainTask.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "genericTiva.h"
#include "mainTiva.h"

#define mainAUTO_RELOAD_TIMER_PERIOD pdMS_TO_TICKS( 2000 )

//Queue handles
QueueHandle_t xMainQueue;
QueueHandle_t xSocketQueue;
QueueHandle_t xLightQueue;
QueueHandle_t xAccelerometerQueue;

/* Handles for the tasks create by main(). */
extern TaskHandle_t xMainTaskHandle;
extern TaskHandle_t xSocketTaskHandle;
extern TaskHandle_t xLightTaskHandle;
extern TaskHandle_t xAccelerometerTaskHandle;
//extern TaskHandle_t xI2cLightTaskHandle;

bool socketFlag = true;
bool lightFlag = true;
bool accelerometerFlag = true;

// Function for main task
void mainTask(void *pvParameters)
{
    TimerHandle_t xMyTimer;
    BaseType_t xMainStatus;
    tiva_msgStruct_t mainReceivedmsg;
    BaseType_t xTimerStarted;
    size_t msgCount = 0;
    UARTprintf("\r\n In the main task");

    const TickType_t xMainTicksToWait = mainAUTO_RELOAD_TIMER_PERIOD;
    xMyTimer = xTimerCreate("mySWTimer",mainAUTO_RELOAD_TIMER_PERIOD,pdTRUE,0,myTimerCallBack );
    if(xMyTimer == NULL)
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

    if( xMyTimer != NULL )
    {
        xTimerStarted = xTimerStart( xMyTimer, 0 );
        if(xTimerStarted == pdPASS )
            UARTprintf("\r\n main():: Timer Started");
        else
            UARTprintf("\r\n main():: Timer Start failed");
    }

    for (;;)
    {
        if(ulTaskNotifyTake(pdFALSE, 0) != 0)
        {
            UARTprintf("\r\n main():: !!!!!!!!!Received Timer notification");
            //*TBD*
            while((msgCount = uxQueueMessagesWaiting(xMainQueue)) != 0)
            {
                xMainStatus = xQueueReceive( xMainQueue, (tiva_msgStruct_t *)&mainReceivedmsg, xMainTicksToWait );
                if( xMainStatus == pdPASS )
                    //*TBD* send logs
                    UARTprintf("\r\n main():: @@@@@@@@@@xQueueReceive Success");
                else
                    //*TBD* send logs
                    UARTprintf("\r\n main():: ##########xQueueReceive Failure");
                if(mainReceivedmsg.tivaMsgId == TIVA_MSGID_HB_RESP)
                {
                    if(mainReceivedmsg.tivaMsgSrcTask == TIVA_SOCKET_TASK_ID)
                        socketFlag = true;
                    else if(mainReceivedmsg.tivaMsgSrcTask == TIVA_LIGHT_TASK_ID)
                        lightFlag = true;
                    else if(mainReceivedmsg.tivaMsgSrcTask == TIVA_ACCELEROMETER_TASK_ID)
                        accelerometerFlag = true;
                }
            }
        }
    }
}

/* Timer Call-Back function*/
static void myTimerCallBack( TimerHandle_t xTimer )
{
    //TickType_t xTimeNow;
    //xTimeNow = uxTaskGetTickCount();    /* Obtain the current tick count. */

    /* Output a string to show the time at which the callback was executed. */
    UARTprintf("\r\n Received Timer signal");
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /*
    if((xTaskNotifyFromISR(xI2cLightTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken) != pdPASS))
        return;*/
#if 1
    if((xTaskNotifyFromISR(xSocketTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken) != pdPASS))
        return;
    if((xTaskNotifyFromISR(xLightTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken) != pdPASS))
        return;
    if((xTaskNotifyFromISR(xAccelerometerTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken) != pdPASS))
        return;
    if((xTaskNotifyFromISR(xMainTaskHandle, 1, eSetBits, &xHigherPriorityTaskWoken) != pdPASS))
        return;

    UARTprintf("\r\n $$$$$$$$$Gave Notify signals from Timer callback");
    if((socketFlag == true) && (lightFlag == true) && (accelerometerFlag == true))
    {
        UARTprintf("\r\n %%%%%%Received HeartBeats");
        socketFlag = false;
        lightFlag = false;
        accelerometerFlag = false;
    }
    else
    {
        if(socketFlag == false)
        {
            UARTprintf("\r\n ^^^^^^^^^^Failed HeartBeats from socket task");
            //*TBD* send error logs
        }
        if(lightFlag == false)
        {
            UARTprintf("\r\n &&&&Failed HeartBeats from Light task");
            //*TBD* send error logs
        }
        if(accelerometerFlag == false)
        {
            UARTprintf("\r\n ****Failed HeartBeats from accelerometer task");
            //*TBD* send error logs
        }
    }
#endif
}

