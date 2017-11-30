/**
 * @file 
 *		socket.c 		*BBG*
 * @brief 
 *		This file contains socket functionalities to communiate with TIVA
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#include "generic.h"
#include "socket.h"

extern bool sigHandle;
mqd_t socket_qdes_log;
	
/* Start routine for the Socket Task */
void *socketTaskFunc(void *arg)
{
	char dummyAmbience[20] = "Day";
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0; 

	mqd_t socket_qdes_main;
	mqd_t socket_qdes_socket;
	mqd_t socket_qdes_decision;
	struct sigevent mq_socketTask_notify; 

	msgStruct_t *read_socket_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_socket = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *socketTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 
    log_t *socketLog = (log_t *)malloc(sizeof(log_t));

    char logPayloadBuff[200];


	/* configuring the mq_notify signals */	
	mq_socketTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_socketTask_notify.sigev_signo = SIGSOCKET;

	if((socket_qdes_main = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the main task queue */
	{
    	printf("socket()::main queue open error is %d\n", errno);
    }

	if((socket_qdes_socket = mq_open(SOCKET_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1)  /* open the socket task queue */
	{
    	printf("socket()::socket queue open error is %d\n", errno);
    }

	if((socket_qdes_decision = mq_open(DECISION_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the decision task queue */
	{
    	printf("socket()::decision queue open error is %d\n", errno);
    }

	if((socket_qdes_log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the log task queue */
	{
    	printf("socket()::log queue open error is %d\n", errno);
    }

	/* Registering notification for message queues */ 
    if(mq_notify(socket_qdes_socket, &mq_socketTask_notify) == -1)							
    {
    	printf("socket()::mq_notify error %d\n", errno);
        if(errno == EBUSY)
            printf("socket()::Another process has registered the Socket queue.\n");
    }

    blockSignals();

	while(!sigHandle)
	{
		recvSig = unblockOnSignal(SOCKET_TASK_ID); 		/* unblock on SIGSOCKET */ 
		
		if(recvSig == SIGHBSOCKET)
		{
			// *TBD* send heartbeat
			printf("socket()::Received HB req\n");     	 	//Remove  
			send_heartBeat(SOCKET_TASK_ID,HB_socket, socket_qdes_main);
		}
		else
		{
			/* Registering notification for message queues */ 
		    if(mq_notify (socket_qdes_socket, &mq_socketTask_notify) == -1)							
		    {
		    	printf("socket()::mq_notify error %d\n", errno);
		        if(errno == EBUSY)
		            printf("socket()::Another process has registered the Socket queue.\n");
		    }
			do
			{
				if((mq_receive(socket_qdes_socket, (char *)read_socket_msg_queue, sizeof(msgStruct_t), NULL)) == -1)  /* receive the msg from queue */
				{
					n = errno;
					printf("socket()::mq_receive error is %d\n", errno);
				} 
				else
				{
					if(read_socket_msg_queue->msgId == MSGID_LIGHT_DATA)			/* If light data from socket request */										
					{
						sprintf(logPayloadBuff, "Ambience: %s", dummyAmbience); 
                		send_log_socket(logPayloadBuff, LOG_INFO, socketLog, socketTaskLogMsg);
					}
					else if(read_socket_msg_queue->msgId == MSGID_TIVA_ALERT)	/* If alert data from socket request */										
					{
						// *TBD* send_log and to main
						sprintf(logPayloadBuff, "ERROR from source %d", read_socket_msg_queue->msgSrcTask); 
                		send_log_socket(logPayloadBuff, LOG_ERROR, socketLog, socketTaskLogMsg);						
					}				
				}
			}while(n != EAGAIN);
			n = 0;
		}
	}

	printf("socket()::Terminating signal received\n");
    
	/* free all the dynamic memory allocated */	
	free(read_socket_msg_queue);
	free(HB_socket);
	free(socketTaskLogMsg);
	free(socketLog);

	/* close all the message queues */
    if (mq_close (socket_qdes_main) == -1) 
    {
        printf("socket()::error no %d closing main queue\n",errno);
    }
    if (mq_close (socket_qdes_socket) == -1) 
    {
        printf("socket()::error no %d closing socket queue\n",errno);
    }
    if (mq_close (socket_qdes_decision) == -1) 
    {
        printf("socket()::error no %d closing decision queue\n",errno);
    }
    if (mq_close (socket_qdes_log) == -1) 
    {
        printf("socket()::error no %d closing log queue\n",errno);
    }

    /* Unlink all the messasges */
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("socket()::error no %d unlinking main queue\n",errno);
    } 
    if (mq_unlink (SOCKET_TASK_MQ_NAME) == -1) 
    {
        printf("socket()::error no %d unlinking socket queue\n",errno);
    } 

    if (mq_unlink (DECISION_TASK_MQ_NAME) == -1) 
    {
        printf("socket()::error no %d unlinking decision queue\n",errno);
    }

    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("socket()::error no %d unlinking log queue\n",errno);
    } 
	
	pthread_exit(NULL);
}

void send_log_socket(char * msg, LOGGER_level socketLogLevel, log_t *socketlogPacket, msgStruct_t *socketMsgPacket)
{
    char socketLogBuff[200];
  	socketlogPacket->logLevel = socketLogLevel;
	socketlogPacket->log_timestamp = time(NULL);
	
	sprintf(socketLogBuff, "Log level: %d, Time: %d, Log: %s",  socketlogPacket->logLevel, socketlogPacket->log_timestamp, (char *)socketlogPacket->logPayload);
	socketlogPacket->logPayload = socketLogBuff;
	send_log(SOCKET_TASK_ID, socketlogPacket, socketMsgPacket, 	socket_qdes_log);
}
