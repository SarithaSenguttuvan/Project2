/**
 * @file 
 *		log.c 		*BBG*
 * @brief 
 *		This file contains logging functionalities to write to a log file
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#include "generic.h"
#include "log.h"

extern bool sigHandle;
extern char *file_name;

/* Start routine for the Log Task */
void *logTaskFunc(void *arg)
{
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0; 
	FILE *fp;								/* File descriptor */
	
	mqd_t log_qdes_main;
	mqd_t log_qdes_log;

	struct sigevent mq_logTask_notify;

	msgStruct_t *read_log_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_log = (msgStruct_t *)malloc(sizeof(msgStruct_t));

	/* configuring the mq_notify signals */	
	mq_logTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_logTask_notify.sigev_signo = SIGLOG;

	if((log_qdes_main = mq_open(MAIN_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the main task queue */
	{
    	printf("log()::main queue open error is %d\n", errno);
    }

	if((log_qdes_log = mq_open(LOG_TASK_MQ_NAME, O_NONBLOCK | O_RDONLY)) == (mqd_t)-1)  /* open the log task queue */
	{
    	printf("log()::log queue open error is %d\n", errno);
    }

	/* Registering notification for message queues */ 
    if(mq_notify (log_qdes_log, &mq_logTask_notify) == -1)							
    {
    	printf("log()::mq_notify error %d\n", errno);
        if(errno == EBUSY)
            printf("log()::Another process has registered the log queue.\n");
    }

    blockSignals();
	fp = fopen(file_name, "w");								/* Open the file name */
    if (fp == NULL) 
    {
      printf("log():: Unable to open the file\n"); 
    }
    printf("Log(): Before while\n");
	while(!sigHandle)
	{
		recvSig = unblockOnSignal(LOG_TASK_ID); 		/* unblock on SIGLOG */ 

		if(recvSig == SIGHBLOG)
		{
			// *TBD* send heartbeat
			printf("log()::Received HB req\n");     	 	//Remove  
			send_heartBeat(LOG_TASK_ID, HB_log, log_qdes_main);
		}
		else
		{
			/* Registering notification for message queues */ 
		    if(mq_notify (log_qdes_log, &mq_logTask_notify) == -1)							
		    {
		    	printf("log()::mq_notify error %d\n", errno);
		        if(errno == EBUSY)
		            printf("log()::Another process has registered the log queue.\n");
		    }
			do
			{
				if((mq_receive(log_qdes_log, (char *)read_log_msg_queue, sizeof(msgStruct_t), NULL)) == -1)  /* receive the msg from queue */
				{
					n = errno;
					printf("log()::mq_receive error is %d\n", errno);
				} 
				else
				{
					if(read_log_msg_queue->msgId == MSGID_LOGMSG)			/* If log message */										
					{
						// * TBD* send log
						printf("log():: Writing to the file\n");
						fprintf(fp, "\nMsg Id: %d | ",read_log_msg_queue->msgId);
						fprintf(fp, "Log level: %d | ",read_log_msg_queue->logLevel);
						fprintf(fp, "Src Task Id: %d | ",read_log_msg_queue->msgSrcTask);
						fprintf(fp, "Payload: %s | ",(char *)(read_log_msg_queue->msgPayload));
						fprintf(fp, "Time: %d | ", time(NULL));
						//fclose(fp); // Remove
					}				
				}
			}while(n != EAGAIN);
			n = 0;
		}
	}

	printf("log()::Terminating signal received\n");

	/* free all the dynamic memory allocated */	
	if(fclose(fp) != 0)
		printf("log():: Error closing the file\n");
	free(read_log_msg_queue);

	/* close all the message queues */
    if (mq_close (log_qdes_main) == -1) 
    {
        printf("log()::error no %d closing main queue\n",errno);
    }
    if (mq_close (log_qdes_log) == -1) 
    {
        printf("log()::error no %d closing log queue\n",errno);
    }

    /* Unlink all the messasges */
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        printf("log()::error no %d unlinking main queue\n",errno);
    } 
    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
        printf("log()::error no %d unlinking log queue\n",errno);
    } 	
	pthread_exit(NULL);
}
