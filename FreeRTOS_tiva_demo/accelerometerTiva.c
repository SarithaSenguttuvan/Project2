/*
 * accelerometer.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "accelerometerTiva.h"
#include "genericTiva.h"
#include "I2C_AccTiva.h"

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
    uint32_t accelerometerNotificationValue = 0;
    uint8_t acc_read_data = 0;
    UARTprintf("\r\n In the accelerometer task");
    i2cAccSetup();
    read_data_acc(0x0D, &acc_read_data, ONE_BYTE);
    read_data_acc(I2C_ACC_CONTROL_REG1, &acc_read_data, ONE_BYTE);
    write_control1_reg_acc(I2C_ACC_CONTROL_REG1_VAL);      //reset to 0 - def value
    read_data_acc(I2C_ACC_CONTROL_REG1, &acc_read_data, ONE_BYTE);
    UARTprintf("\r\n The ID value is ");
    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &accelerometerNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            if((accelerometerNotificationValue & HB_REQ_BIT) == SET_BIT)
            {
                sendHB(TIVA_ACCELEROMETER_TASK_ID, &accelerometerHBMsg);
                UARTprintf("\r\n Accelerometer():: ?????Received HB request");
                //*TBD* send HB
                //xSocketStatus = xQueueSendToBack( xMainQueue, &socketHBMsg, 0 );
            }
        }
        else
        {
            UARTprintf("\r\n Accelerometer():: HB request receive error");
        }
    }
}


