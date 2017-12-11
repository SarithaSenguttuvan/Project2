/**
 * @file 
 *		generic.c 		*BBG*
 * @brief 
 *		This file contains functionalities common to all the tasks 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */


#include "generic.h"

/* Function to block all the signals */
void blockSignals()
{
	int8_t rc = 0;
	sigset_t globalMask_set;
	sigemptyset(&globalMask_set);
	sigaddset(&globalMask_set, SIGMAIN);
	sigaddset(&globalMask_set, SIGTIMER);
	sigaddset(&globalMask_set, SIGDECISION);
	sigaddset(&globalMask_set, SIGLOG);
	sigaddset(&globalMask_set, SIGGESTURE);	
	sigaddset(&globalMask_set, SIGHBSOCKET);
	sigaddset(&globalMask_set, SIGSOCKET);	
	sigaddset(&globalMask_set, SIGHBDECISION);
	sigaddset(&globalMask_set, SIGHBLOG);
	sigaddset(&globalMask_set, SIGHBGESTURE);
	//sigaddset(&globalMask_set, SIGINT);
	rc = pthread_sigmask(SIG_BLOCK, &globalMask_set, NULL);
	if (rc != 0)
   		printf("generic::Sigmask Block error\n");
	printf("generic::Blocked Signals\n");
}

/* Unblock the signals based on the task that requests */
int32_t unblockOnSignal(int taskID)
{
	int8_t rc = 0;
	int32_t sigNo = 0;
	sigset_t globalMask_set;
	sigemptyset(&globalMask_set);
	if(taskID == MAIN_TASK_ID)				/* if main task */	
	{
		sigaddset(&globalMask_set, SIGMAIN);
		sigaddset(&globalMask_set, SIGTIMER);
		//sigaddset(&globalMask_set, SIGINT);
		printf("unblockSignals::Main\n");
	}
	else if(taskID == LOG_TASK_ID)		/* If log task */
	{
		sigaddset(&globalMask_set, SIGLOG);
		sigaddset(&globalMask_set, SIGHBLOG);
		printf("unblockSignals::LOG\n");
	}
	else if(taskID == SOCKET_TASK_ID)		/* if Socket task */
	{
		sigaddset(&globalMask_set, SIGSOCKET);
		sigaddset(&globalMask_set, SIGHBSOCKET);
		printf("unblockSignals::Socket\n");
	}
	else if(taskID == DECISION_TASK_ID)		/* if Decision task */
	{
		sigaddset(&globalMask_set, SIGDECISION);
		sigaddset(&globalMask_set, SIGHBDECISION);
		printf("unblockSignals::Decision\n");
	}
	else if(taskID == GESTURE_TASK_ID)		/* if Gesture task */
	{
		sigaddset(&globalMask_set, SIGGESTURE);
		sigaddset(&globalMask_set, SIGHBGESTURE);
		printf("unblockSignals::Gesture\n");
	}	
   	if(sigwait(&globalMask_set, &sigNo))			/* wait on the signals based on the signal mask */
   	{
   		printf("sigwait_error in TaskId %d : %d", taskID, errno);
   		return -1;
   	}
   	else
   	{
   		return sigNo;			/* Return the signal number */
   	}
}

/* Function call to send the heartBeat signal */
uint8_t send_heartBeat(int8_t srcTaskId, msgStruct_t *HB_main, mqd_t mq_des_hb_resp)
{
	int8_t rc = 0;
	printf("send_heartBeat::In function send HB\n");

	HB_main->msgId = MSGID_HB_RESP;
	HB_main->msgSrcTask = srcTaskId;
	HB_main->logLevel = LOG_NONE;
	HB_main->msgPayload = NULL;
	HB_main->msgPayloadLen = 0;
           
	if(mq_send(mq_des_hb_resp, (char *)HB_main, sizeof(msgStruct_t), 0) == -1) 		/* Send heartbeat to main task */
    {
    	printf ("send_heartBeat::Error no %d, from task %d\n", errno, srcTaskId);
    }
    else
    	printf("send_heartBeat::HB Sent %d\n", srcTaskId);  
    return 0;
}

/* Send log to the log task */
uint8_t send_log(uint8_t srcTaskId, LOGGER_level LogLevel, char* logMsg, msgStruct_t *msg, mqd_t msg_qdes)   
{
	msg->msgId = MSGID_LOGMSG;
	msg->msgSrcTask = srcTaskId;
	msg->logLevel = LogLevel;
	msg->msgPayload = logMsg;
	msg->msgPayloadLen = strlen(logMsg);

    if(mq_send(msg_qdes, (char *)msg, sizeof(msgStruct_t), 0) == -1) 
    {
    	printf ("send_log::ERROR No: %d from task ID %d\n", errno, srcTaskId);
    }
    else
    	printf("send_log::Log message Sent from task %d\n", srcTaskId);

	return 0;
}
