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

/* Handles for the tasks create by main(). */
extern TaskHandle_t xSocketTaskHandle;

// Function for Light task
void lightTask(void *pvParameters)
{
    UARTprintf("\r\n In the Light task");
    tiva_msgStruct_t lightHBMsg;
    uint32_t lightNotificationValue = 0;
    tiva_msgStruct_t lightData;
    bool test_ambience = false;
    bool current_ambience = test_ambience;
    uint8_t power_value = 0;
    i2cLightSetup();
   write_control_reg(I2C_CONTROL_REG_POWER);   /* Write to the control register for powering the device */
    read_data(I2C_CONTROL_REG, &power_value, ONE_BYTE);     /* Read and verify the control register value */

    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &lightNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            UARTprintf("\r\n light():: !!!!!!!!!Received Timer notification");
            if((lightNotificationValue & HB_REQ_BIT) == SET_BIT)
            {
                sendHB(TIVA_LIGHT_TASK_ID, &lightHBMsg);
                light_dark(&test_ambience);     /* API to verify the current ambience */
                if(current_ambience != test_ambience)
                {
                    //send to socket
                    if(test_ambience)
                        send_socket_light("Day$",&lightData);
                    else
                        send_socket_light("Dark$",&lightData);

                    if((xTaskNotify(xSocketTaskHandle, 2, eSetBits) != pdPASS))
                            return;
                    current_ambience = test_ambience;

#if LIGHT_DATA_TEST
                   uint16_t data0 = 2;
                   uint16_t data1 = 1;
                   float results_obtained = 0.0;
                   float actual_value = 0.060800;
                   lux_calculate(data0, data1,&results_obtained);
                   if((results_obtained-actual_value) == 0)
                   {
                       UARTprintf("\r\n Test Passed");
                   }
#endif                    
                }
                if(test_ambience == false)
                {
                    //LEDWrite(0x0F, 0x04);
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 1);       /* If dark, LED on */
                }
                else
                {
                    //LEDWrite(0x0F, 0x00);
                    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, 0);       /* If light, LED off */
                }
            }
        }
        else
        {
            UARTprintf("\r\n Light():: HB request receive error");
        }
    }
}

void send_socket_light(char * msg, tiva_msgStruct_t *socketPacket)
{
    socketPacket->tivaMsgId = TIVA_MSGID_LIGHT_DATA;
    socketPacket->tivaMsgSrcTask = TIVA_LIGHT_TASK_ID;
    socketPacket->tivaMsgPayloadLen = 0;
    socketPacket->tivaMsgPayload = msg;
    BaseType_t xLightStatus;
    xLightStatus = xQueueSendToBack( xSocketQueue, (void *)socketPacket, 0 );
    if( xLightStatus == pdPASS )
    {
        UARTprintf("\r\nLight:: Sent to queue");
    }
}
