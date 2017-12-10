/*
 * lightTiva.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "lightTiva.h"
#include "genericTiva.h"
#include "I2C_LightTiva.h"

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
    uint32_t lightNotificationValue = 0;

    bool test_ambience = false;
    uint8_t power_value = 0;
    i2cLightSetup();
    read_data(I2C_ID_REG, &power_value, ONE_BYTE);     /* Read and verify the control register value */
    read_data(I2C_CONTROL_REG, &power_value, ONE_BYTE);
    write_control_reg(I2C_CONTROL_REG_POWER);   /* Write to the control register for powering the device */


    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &lightNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            if((lightNotificationValue & HB_REQ_BIT) == SET_BIT)
            {
                sendHB(TIVA_LIGHT_TASK_ID, &lightHBMsg);
                light_dark(&test_ambience);     /* API to verify the current ambience */
                if(test_ambience == false)
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 1);       /* If dark, LED on */
                else
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);       /* If light, LED off */
            }
        }
        else
        {
            UARTprintf("\r\n Light():: HB request receive error");
        }
    }
}


