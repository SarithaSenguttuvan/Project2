/**
 * @file 
 *		decision.c 		*BBG*
 * @brief 
 *		This file conatains functionalities with respect to decision task
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#include "generic.h"
#include "decision.h"

extern bool sigHandle;

/* Start routine for the Socket Task */
void *decisionTaskFunc(void *arg)
{
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0; 

	mqd_t decision_qdes_main;
	mqd_t decision_qdes_socket;
	mqd_t decision_qdes_decision;
	mqd_t decision_qdes_log;
	struct sigevent mq_decisionTask_notify;

	msgStruct_t *read_decision_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_decision = (msgStruct_t *)malloc(sizeof(msgStruct_t));

	/* configuring the mq_notify signals */	
	mq_decisionTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_decisionTask_notify.sigev_signo = SIGDECISION;

	if((decision_qdes_main = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the main task queue */
	{
    	printf("decision()::main queue open error is %d\n", errno);
    }

	if((decision_qdes_socket = mq_open(SOCKET_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the socket task queue */
	{
    	printf("decision()::socket queue open error is %d\n", errno);
    }

	if((decision_qdes_decision = mq_open(DECISION_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1)  /* open the decision task queue */
	{
    	printf("decision()::decision queue open error is %d\n", errno);
    }

	if((decision_qdes_log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the log task queue */
	{
    	printf("decision()::log queue open error is %d\n", errno);
    }

	/* Registering notification for message queues */ 
    if(mq_notify (decision_qdes_decision, &mq_decisionTask_notify) == -1)							
    {
    	printf("decision()::mq_notify error %d\n", errno);
        if(errno == EBUSY)
            printf("decision()::Another process has registered the decision queue.\n");
    }

    blockSignals();
    printf("Decision(): Before while\n");
	while(!sigHandle)
	{
		recvSig = unblockOnSignal(DECISION_TASK_ID); 		/* unblock on SIGDECISION */ 
		if(recvSig == SIGHBDECISION)
		{
			// *TBD* send heartbeat
			printf("decision()::Received HB req\n");     	 	//Remove  
			send_heartBeat(DECISION_TASK_ID,HB_decision, decision_qdes_main);
		}
		else
		{
			/* Registering notification for message queues */ 
		    if(mq_notify (decision_qdes_socket, &mq_decisionTask_notify) == -1)							
		    {
		    	printf("decision()::mq_notify error %d\n", errno);
		        if(errno == EBUSY)
		            printf("socket()::Inside If other process has registered the decision queue.\n");
		    }
			do
			{
				if((mq_receive(decision_qdes_socket, (char *)read_decision_msg_queue, sizeof(msgStruct_t), NULL)) == -1)  /* receive the msg from queue */
				{
					n = errno;
					printf("decision()::mq_receive error is %d\n", errno);
				} 
				else
				{
					if(read_decision_msg_queue->msgId == MSGID_SCKT_DATA)			/* If socket data from socket request */										
					{
						// * TBD* if error send to main and log
					}			
				}
			}while(n != EAGAIN);
			n = 0;
		}
	}

	printf("decision()::Terminating signal received\n");

	/* free all the dynamic memory allocated */	
	free(read_decision_msg_queue);
	free(HB_decision);

	/* close all the message queues */
    if (mq_close (decision_qdes_main) == -1) 
    {
        printf("decision()::error no %d closing main queue\n",errno);
    }
    if (mq_close (decision_qdes_socket) == -1) 
    {
        printf("decision()::error no %d closing socket queue\n",errno);
    }
    if (mq_close (decision_qdes_decision) == -1) 
    {
        printf("decision()::error no %d closing decision queue\n",errno);
    }
    if (mq_close (decision_qdes_log) == -1) 
    {
        printf("decision()::error no %d closing log queue\n",errno);
    }

    /* Unlink all the messasges */
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("decision()::error no %d unlinking main queue\n",errno);
    } 
    if (mq_unlink (SOCKET_TASK_MQ_NAME) == -1) 
    {
        printf("decision()::error no %d unlinking socket queue\n",errno);
    } 

    if (mq_unlink (DECISION_TASK_MQ_NAME) == -1) 
    {
        printf("decision()::error no %d unlinking decision queue\n",errno);
    }

    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("decision()::error no %d unlinking log queue\n",errno);
    } 
	
	pthread_exit(NULL);
}
