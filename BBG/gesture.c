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
	msgStruct_t *gestureTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 
    log_t *gestureLog = (log_t *)malloc(sizeof(log_t));

    char logPayloadBuff[200];	
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


    blockSignals();

	while(!sigHandle)
	{
		/* if gesture value send to socket task*/
		/* if HB_req send send HB */
		/*if(read_log_msg_queue->msgId == MSGID_HB_REQ)												
			{
				// *TBD* send_heartBeat(LIGHT_TASK_ID,HB_main, qdes_loc_lightMainQueue);	
			}*/
			
		recvSig = unblockOnSignal(GESTURE_TASK_ID); 		/* unblock on SIGDECISION */ 
		if(recvSig == SIGHBGESTURE)
		{
			// *TBD* send heartbeat
			printf("gesture()::Received HB req\n");     	 	//Remove  
			send_heartBeat(GESTURE_TASK_ID, HB_gesture, gesture_qdes_main);
			 
			if((read_pdata_reg(&pdata_value)) == I2C_SUCCESS)
			{
				printf("The pdata_value reg value is %x\n",pdata_value);
			}
			else
			{
				printf("Error in reading pdata_value value\n");
			}
			
			//read pdata
		}
		else
		{	
			
		}
	}
	printf("gesture()::Terminating signal received\n");
	/* Free memory */
	free(read_log_msg_queue);
	free(HB_gesture);
	free(gestureTaskLogMsg);
	free(gestureLog);
	
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

void send_log_gesture(char * msg, LOGGER_level gestureLogLevel, log_t *gesturelogPacket, msgStruct_t *gestureMsgPacket)
{
    char gestureLogBuff[200];
  	gesturelogPacket->logLevel = gestureLogLevel;
	gesturelogPacket->log_timestamp = time(NULL);
	
	sprintf(gestureLogBuff, "Log level: %d, Time: %d, Log: %s",  gesturelogPacket->logLevel, gesturelogPacket->log_timestamp, (char *)gesturelogPacket->logPayload);
	gesturelogPacket->logPayload = gestureLogBuff;
	send_log(MAIN_TASK_ID, gesturelogPacket, gestureMsgPacket, gesture_qdes_log);
}
