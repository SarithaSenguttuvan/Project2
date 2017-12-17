/**
 * @file 
 *		gesture.c 		*BBG*
 * @brief 
 *		This file is used for geture sensor
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#include "generic.h"
#include "gesture.h"
#include "apds_proximity.h"

extern bool sigHandle;
mqd_t gesture_qdes_log;

/* Start routine for the Log Task */
void *gestureTaskFunc(void *arg)
{
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0; 
	uint8_t pdata_value = 0; 		/* Proximity sensor value */

	mqd_t gesture_qdes_main;
	mqd_t gesture_qdes_socket;

	msgStruct_t *read_log_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_gesture = (msgStruct_t *)malloc(sizeof(msgStruct_t));

	/* Gesture notification packet */
    msgStruct_t *pdata_struct = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	pdata_struct->msgId = MSGID_PROX_NOTIFY;
	pdata_struct->msgSrcTask = GESTURE_TASK_ID;
		
	if((gesture_qdes_main = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the main task queue */
	{
    	printf("gesture()::main queue open error is %d\n", errno);
    }

	if((gesture_qdes_socket = mq_open(SOCKET_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the socket task queue */
	{
    	printf("gesture()::socket queue open error is %d\n", errno);
    }

	if((gesture_qdes_log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the LOG task queue */
	{
    	printf("gesture()::log queue open error is %d\n", errno);
    }

    /* Proximity sensor setup */
    if(proximity_setup() == I2C_SUCCESS)
	{
		printf("Gesture:: Configured I2C\n");
	}
	else
	{
		printf("Gesture:: Error in configuring I2C\n");
	}
	
	write_control_value(CONTROL_REG_VAL);			//Setting the PGAIN value. when set to 0 got the pdata as 22, But wen set as 0x0C got the output as 0xF0 and more
	write_pulse_len_count_value(PULSE_LEN_REG_VAL);  //setting the PPLEN value to 32us
	write_enable_value(ENABLE_PON_PEN);			// Enable PON and PEN 
#if TEST_GESTURE
    uint8_t id_value;
    if(read_id_value(&id_value) == 0xAB)
    {
        printf("Test Passed!\n");
    }
#endif
	send_log_gesture("Proximity Sensor initialized", LOG_INIT, read_log_msg_queue);
    blockSignals();
    printf("Gesture(): Before while\n");
	while(!sigHandle)
	{
		recvSig = unblockOnSignal(GESTURE_TASK_ID); 		/* unblock on SIGDECISION */ 
		if(recvSig == SIGHBGESTURE)
		{
			printf("gesture()::Received HB req\n");     	 	
			send_heartBeat(GESTURE_TASK_ID, HB_gesture, gesture_qdes_main);
			 
			if((read_pdata_reg(&pdata_value)) == I2C_SUCCESS)
			{
				printf("The pdata_value reg value is %x\n",pdata_value);
			}
			else
			{
				printf("Error in reading pdata_value value\n");
			}
			
			//pdata_value = 25; 			//TBD Dummy pdata value

			pdata_struct->int_data = pdata_value;
			if((pdata_value) > PROX_THRESHOLD)
			{
				printf("Above Threshold\n");
				if(mq_send(gesture_qdes_socket, (char *)pdata_struct, sizeof(msgStruct_t), 0) == -1) 		/* Send heartbeat to main task */
				{
					printf ("Gesture::Error in sending the pdata value, Error no %d, from task\n", errno);
				}
				else
				{
					printf("Gesture::Sent pdata value\n");
				}	
				send_log_gesture("Near Object detected", LOG_INFO, read_log_msg_queue);
			}
		}
	}
	printf("gesture()::Terminating signal received\n");
	/* Free memory */
	free(read_log_msg_queue);
	free(HB_gesture);
	
	/* close all the message queues */
    if (mq_close (gesture_qdes_main) == -1) 
    {
        printf("gesture()::error no %d closing main queue\n",errno);
    }
    if (mq_close (gesture_qdes_socket) == -1) 
    {
        printf("gesture()::error no %d closing socket queue\n",errno);
    }

    /* Unlink all the messasges */
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("gesture()::error no %d unlinking main queue\n",errno);
    } 
    if (mq_unlink (SOCKET_TASK_MQ_NAME) == -1) 
    {
        printf("gesture()::error no %d unlinking socket queue\n",errno);
    } 	
	pthread_exit(NULL);
}

void send_log_gesture(char * msg, LOGGER_level gestureLogLevel, msgStruct_t *gestureMsgPacket)
{
	send_log(GESTURE_TASK_ID, gestureLogLevel, msg, gestureMsgPacket, gesture_qdes_log);	
}
