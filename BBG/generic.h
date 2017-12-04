/** NG
 * @file 
 *		generic.h 
 * @brief 
 *		This file contains function declarations for variables used in generic.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#ifndef INCLUDES_GENERIC_TASK_H_
#define INCLUDES_GENERIC_TASK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <stdbool.h>
#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>

#include "log.h"



/* Enum for type of message i.e., Message ID */
typedef enum
{
	MSGID_HB_REQ,
	MSGID_HB_RESP,
	MSGID_LIGHT_REQ,
	MSGID_LIGHT_DATA,
	MSGID_ACCELEROMETER_DATA,
	MSGID_LIGHT_AUTO,
	MSGID_LIGHT_CTRL,
	MSGID_LOGMSG,
	MSGID_SCKT_DATA,
	MSGID_TIVA_ALERT,
	MSGID_ERROR
}msgid_t;

/* Structure for a message */
typedef struct
{
	msgid_t msgId;
	uint8_t msgSrcTask;				/* Source Task ID */
	size_t msgPayloadLen;			/* Msg length  */
	void * msgPayload; 				/* Msg associated data */
}msgStruct_t;

/* Queue names */
#define MAIN_TASK_MQ_NAME "/main_task_queue"

#define SOCKET_TASK_MQ_NAME "/socket_task_queue"

#define DECISION_TASK_MQ_NAME "/decision_task_queue"

#define LOG_TASK_MQ_NAME "/log_task_queue"

#define MAIN_TASK_ID 1
#define SOCKET_TASK_ID 2
#define DECISION_TASK_ID 3
#define LOG_TASK_ID 4
#define GESTURE_TASK_ID 5

/* Signal Macros */ 
#define SIGMAIN 50
#define SIGSOCKET 51	
#define SIGDECISION 52
#define SIGLOG 53
#define SIGGESTURE 54
#define SIGTIMER 55

#define SIGHBSOCKET 56
#define SIGHBDECISION 57
#define SIGHBLOG 58
#define SIGHBGESTURE 59

/**
 * @brief 
 * 		This function is used to block the signals
 * @return 
 *      none
 */
void blockSignals();

/**
 * @brief 
 * 		This function is used to unblock the signals
 * @return 
 *      int32_t
 */
int32_t unblockOnSignal(int taskID);

/**
 * @brief 
 * 		This function is used to send heartbeat messages to the main queue
 * @param 
 *		srcTask - Holds the sourcetask ID that sends the logs
 *		HB_main - holds the pointer to the heartBeat msg structure
 *		mq_des_hb_resp - Is the queue descriptor of the main task queue
 * @return 
 *      uint8_t
 */
uint8_t send_heartBeat(int8_t srcTaskId, msgStruct_t *HB_main, mqd_t mq_des_hb_resp);

/**
 * @brief 
 * 		This function is used to send logs to the log queue
 * @param 
 *		srcTaskId - Holds the sourcetask ID that sends the logs
 *		logPacket - holds the log packet of type log_t
 * @return 
 *      uint8_t
 */
uint8_t send_log(uint8_t srcTaskId, log_t * logPacket, msgStruct_t *msg, mqd_t msg_qdes);		//Frame the message of type msgStruct_t in this function



#endif
