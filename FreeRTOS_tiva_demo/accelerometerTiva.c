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

extern TaskHandle_t xSocketTaskHandle;

#define ACCELEROMETER_TASK_WAIT_TIME   pdMS_TO_TICKS( 2000 )

// Function for accelerometer task
void accelerometerTask(void *pvParameters)
{
    tiva_msgStruct_t accelerometerHBMsg;
    tiva_msgStruct_t accData;
    uint32_t accelerometerNotificationValue = 0;
    uint8_t acc_read_data = 0;
    UARTprintf("\r\n In the accelerometer task");

    i2cAccSetup();
    read_data_acc(I2C_ACC_ID_REG, &acc_read_data, ONE_BYTE);
    write_control2_reg_acc(I2C_ACC_CONTROL_REG2_VAL);
    read_data_acc(I2C_ACC_CONTROL_REG2_VAL, &acc_read_data, ONE_BYTE);
    write_ff_mt_cfg_reg_acc(I2C_FF_MT_CFG_VAL);
    read_data_acc(I2C_FF_MT_CFG_VAL, &acc_read_data, ONE_BYTE);
    write_ff_mt_ths_reg_acc(I2C_FF_MT_THS_VAL);
    read_data_acc(I2C_FF_MT_THS_VAL, &acc_read_data, ONE_BYTE);
    write_ff_mt_count_reg_acc(I2C_FF_MT_COUNT_VAL);
    read_data_acc(I2C_FF_MT_COUNT_VAL, &acc_read_data, ONE_BYTE);
    write_control4_reg_acc(I2C_ACC_CONTROL_REG4_VAL);
    read_data_acc(I2C_ACC_CONTROL_REG4_VAL, &acc_read_data, ONE_BYTE);
    write_control1_reg_acc(I2C_ACC_CONTROL_REG1_VAL);
    read_data_acc(I2C_ACC_CONTROL_REG1_VAL, &acc_read_data, ONE_BYTE);

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

                read_data_acc(I2C_INT_SOURCE, &acc_read_data, ONE_BYTE);
                if(acc_read_data == 0x04)
                {
                    send_socket_acc("DoorOpened$",&accData);
                    if((xTaskNotify(xSocketTaskHandle, 2, eSetBits) != pdPASS))
                                            return;
                }
/*
                //read data from a register
                //if (acc_read())
                {
                    send_socket_acc("DoorOpened$",&accData);
                    if((xTaskNotify(xSocketTaskHandle, 2, eSetBits) != pdPASS))
                        return;
                }
*/
            }
        }
        else
        {
            UARTprintf("\r\n Accelerometer():: HB request receive error");
        }
    }
}

void send_socket_acc(char * msg, tiva_msgStruct_t *socketPacket)
{
    socketPacket->tivaMsgId = TIVA_MSGID_ACCELEROMETER_DATA;
    socketPacket->tivaMsgSrcTask = TIVA_ACCELEROMETER_TASK_ID;
    socketPacket->tivaMsgPayloadLen = 0;
    socketPacket->tivaMsgPayload = msg;
    BaseType_t xAccStatus;
    xAccStatus = xQueueSendToBack( xSocketQueue, (void *)socketPacket, 0 );
    if( xAccStatus == pdPASS )
    {
        UARTprintf("\r\n Accelerometer:: Sent to queue");
    }
}

