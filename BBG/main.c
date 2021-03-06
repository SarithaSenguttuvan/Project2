/**
 * @file 
 *		main.c 		*BBG*
 * @brief 
 *		This file is the main task which spawns other tasks on BBG
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#include "main.h"
#include "generic.h"
#include <sys/stat.h>
#include <time.h>
#include "socket.h"
#include "log.h"
#include "gesture.h"
#include <errno.h>

#define BUFFER_LENGTH 256 

bool sigHandle = false;
const char *file_name;

mqd_t qdes_logTask;
	
/* Queue attributes */
struct mq_attr main_queue_attr;
struct mq_attr socket_queue_attr;
struct mq_attr log_queue_attr;

/* Defining my owm handler to handle the system signals */
void my_handler(int signum)
{
    if((signum == SIGINT))               /* If SIGTERM signal */
    {
        int ret, file_descriptor_led;   
        int query_read;
        int LED_period ;
        int LED_dutyCycle;
        int LED_Toggle;
        char stringToSend[BUFFER_LENGTH];
#if 0
        file_descriptor_led = open("/dev/LED_CharDriver", O_RDWR);             // Open the device with read/write access
        if (file_descriptor_led < 0)
        {
          printf("unable to open the device. Error no: %d\n", errno);
          
          return;
        }

         /*LED Code */
        LED_period = 5000;
        LED_dutyCycle = 2000; 
        LED_Toggle = true;
        query_read = 2;
        sprintf(stringToSend, "%d %d %d %d", LED_period, LED_dutyCycle, LED_Toggle, query_read); 
        ret = write(file_descriptor_led, stringToSend, strlen(stringToSend));       // Write the string to the kernel module
        if (ret < 0)
        {
            perror("Unable to write to the device.\n");
            return;
        }
        close(file_descriptor_led);
#endif
        printf("Received SIGINT!\n");
        sigHandle = true;
        pthread_kill(pthread_self(), SIGTIMER);
        pthread_kill(socketTask, SIGHBSOCKET);
        pthread_kill(gestureTask, SIGHBGESTURE);
        pthread_kill(logTask, SIGHBLOG);		
    }
}
	
/* main function */
int main(int argc, char const *argv[])
{
	uint8_t rc = 0;
	size_t n = 0;
	int32_t recvSig = 0;
	uint8_t taskID;

	bool socket_flag = true;
	bool log_flag = true;
	bool gesture_flag = true;

	mqd_t qdes_mainTask;
	mqd_t qdes_socketTask;
	mqd_t qdes_gestureTask;
	
	file_name = argv[1];

	/* Timer variables */
	timer_t timerid;
    struct itimerspec its;

    /* sigevents registeration for SIGTIMER and SIGMAIN */	
	mq_mainTask_notify.sigev_notify = SIGEV_SIGNAL;						
	mq_mainTask_notify.sigev_signo = SIGMAIN;	    
   	mq_mainTask_timer.sigev_notify = SIGEV_SIGNAL;
    mq_mainTask_timer.sigev_signo = SIGTIMER;

    /* Setting timer value */
    its.it_value.tv_sec = 2;
    its.it_value.tv_nsec = 100 % 1000000000;
    its.it_interval.tv_sec = its.it_value.tv_sec;
    its.it_interval.tv_nsec = its.it_value.tv_nsec;
    
    /* Message and Log packets */
	msgStruct_t *read_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *mainTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 

    char logPayloadBuff[200];
#if 0    
    /* LED code */
    int ret, file_descriptor_led;   
    int query_read;
    int LED_period ;
    int LED_dutyCycle;
    int LED_Toggle;
    char stringToSend[BUFFER_LENGTH];

    file_descriptor_led = open("/dev/LED_CharDriver", O_RDWR);             // Open the device with read/write access
    if (file_descriptor_led < 0)
    {
      printf("unable to open the device. Error no: %d\n", errno);
      
      return errno;
    }
#endif    
	struct sigaction my_sig;		/* Registering signal for Terminating signals */
    my_sig.sa_handler = my_handler; /* Assigning the signal handler function */
    sigaction(SIGINT, &my_sig, NULL);  /* Registering the signals */    

    
    /* Timer create */ 
    if (timer_create(CLOCK_REALTIME, &mq_mainTask_timer, &timerid) == -1)
        printf("main::timer_create error\n");
    printf("main::Timer Created\n");

	/* Main task message queue config and create, notification config and register */ 
    main_queue_attr.mq_maxmsg = 30;
	main_queue_attr.mq_msgsize = sizeof(msgStruct_t);        
	if((qdes_mainTask = mq_open(MAIN_TASK_MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, S_IRUSR, &main_queue_attr)) == -1) 
    {
    	printf("main::main queue open error is %d\n", errno);
    } 
    else
	    printf("main::Main queue opened\n");

	/* Registering notification for message queues */ 
    if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)							
    {
    	printf("main::mq_notify error %d\n", errno);
        if(errno == EBUSY)
           printf("main::Another process has registered for Main queue.\n");
    } 

    /* socket task message queue config and create */ 
	socket_queue_attr.mq_maxmsg = 30;
	socket_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_socketTask = mq_open(SOCKET_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &socket_queue_attr)) == -1) 
    {
    	printf("main::socket queue open error is %d\n", errno);
    } 
	printf("main::socket queue opened\n");
#if 0
    /* Decision task message queue config and create */ 
	decision_queue_attr.mq_maxmsg = 30;
	decision_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_decisionTask = mq_open(DECISION_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &decision_queue_attr)) == -1) 
    {
    	printf("main::Decision queue open error is %d\n", errno);
    } 
	printf("main::Decision queue opened\n");
#endif


    /* Log task message queue config and create */ 
	log_queue_attr.mq_maxmsg = 30;
	log_queue_attr.mq_msgsize = sizeof(msgStruct_t);
	if((qdes_logTask = mq_open(LOG_TASK_MQ_NAME, O_RDWR | O_CREAT | O_NONBLOCK, S_IRWXU, &log_queue_attr)) == -1) 
    {
    	printf("main::Log queue open error is %d\n", errno);
    } 
	printf("main::Log queue opened\n");

    /* Initialising the thread attributes */
    pthread_attr_init(&socketTask_attr);	
    pthread_attr_init(&logTask_attr);
    pthread_attr_init(&gestureTask_attr);

    blockSignals();		/* Block all signals */

    rc = pthread_create(&socketTask, &socketTask_attr, socketTaskFunc, NULL);  		/* Create Socket thread  */
    if(rc) 
    {
        printf("main::pthread_create() socket task error, rc is %d\n", rc); 
        exit(-1);
    }
#if 0
    rc = pthread_create(&decisionTask, &decisionTask_attr, decisionTaskFunc, NULL); /* Create Decision thread  */
    if(rc) 
    {
        printf("main::pthread_create() decision task error, rc is %d\n", rc); 
        exit(-1);
    }
#endif
    
    rc = pthread_create(&logTask, &logTask_attr, logTaskFunc, NULL);  		/* Create log thread  */
    if(rc) 
    {
        printf("main::pthread_create() log task error, rc is %d\n", rc); 
        exit(-1);
    }	

    rc = pthread_create(&gestureTask, &gestureTask_attr, gestureTaskFunc, NULL); /* Create Gesture thread  */
    if(rc) 
    {
        printf("main::pthread_create() gesture task error, rc is %d\n", rc); 
        exit(-1);
    }

	sleep(2); 		/* Sleep for the threads to get created */

    send_log_main("Main task initialized", LOG_INIT, mainTaskLogMsg);

	/* Start the Timer */
    if (timer_settime(timerid, 0, &its, NULL) == -1)
         printf("main::timer_settime error\n");

	while(!sigHandle)				/* Run continuously */
	{	
		recvSig = unblockOnSignal(MAIN_TASK_ID);   				/* unblock on timer and mq_notify signal */
        printf("main:::Received Signal after Sigwait:%d\n", recvSig);
        
        if(recvSig == SIGTIMER)			/* If timer elapses */
        {        	  
        	printf("main()::Received SIGTIMER\n");     	 	//Remove      	
       		if(socket_flag == true && log_flag == true && gesture_flag == true) /* If heartbeats received from all tasks */
            {
            	printf("main()::Received HBs\n");     	 	//Remove  
                socket_flag = false;
                log_flag = false;
                gesture_flag = false;
                pthread_kill(socketTask, SIGHBSOCKET);
                //pthread_kill(decisionTask, SIGHBDECISION);
                pthread_kill(logTask, SIGHBLOG); 
                pthread_kill(gestureTask, SIGHBGESTURE); 
                printf("main()::After received HBs\n"); 
                 
                send_log_main("Received Heartbeats", LOG_INFO, mainTaskLogMsg);
            }
        	else					/* If heartbeats not received */
            {
                if(socket_flag == false)
                {
                    taskID = SOCKET_TASK_ID;
                    printf("HB not received from socket task\n");
                }            
                if(log_flag == false)
                {
                    taskID = LOG_TASK_ID;
                    printf("HB not received from log task\n");
                }
                else if(gesture_flag == false)
                {
                    taskID = GESTURE_TASK_ID;
                    printf("HB not received gesture temp task\n");
                }
                printf("main()::Received faulty HBs\n"); 	//Remove

#if 0
                 /*LED Code */
                LED_period = 5000;
                LED_dutyCycle = 2000; 
                LED_Toggle = true;
                query_read = 2;
                sprintf(stringToSend, "%d %d %d %d", LED_period, LED_dutyCycle, LED_Toggle, query_read); 
                ret = write(file_descriptor_led, stringToSend, strlen(stringToSend));       // Write the string to the kernel module
                if (ret < 0)
                {
                    perror("Unable to write to the device.\n");
                    return errno;
                }
#endif

				sprintf(logPayloadBuff, "ERROR from source %d", taskID); 
                send_log_main(logPayloadBuff, LOG_ERROR, mainTaskLogMsg);
                //*TBD* wait for log to be sent
                //raise(SIGINT);
            }
        }
        else if(recvSig == SIGMAIN)			/* If data received on main task message queue */
        { 	
        	printf("main()::Received SIGMAIN\n");     	 	//Remove  
        	if(mq_notify (qdes_mainTask, &mq_mainTask_notify) == -1)	/* Re-registering for message queus */		
			{
		    	printf("main::mq_notify error %d\n", errno);
		        if(errno == EBUSY)
		            printf("main::Another process has registered for Main queue.\n");
			} 	
			do
			{
				if((mq_receive(qdes_mainTask, (char *)read_queue, sizeof(msgStruct_t), NULL)) == -1) 	/* receive from queue */
				{
					n = errno;
					printf("main::mq_receive error no is %d\n", errno);
				} 
				else
				{
					if(read_queue->msgId == MSGID_HB_RESP)	/* if heartbeat response */				
					{
						printf("main::HB MSG from Source ID is %d\n",(read_queue->msgSrcTask));
						if(read_queue->msgSrcTask == SOCKET_TASK_ID)
							socket_flag = true;
						else if(read_queue->msgSrcTask == LOG_TASK_ID)
							log_flag = true;							
						else if(read_queue->msgSrcTask == GESTURE_TASK_ID)
							gesture_flag = true;	
					}
					else if(read_queue->msgId == MSGID_ERROR)
					{
						//printf("main::Received error message from source ID %d\n", read_queue->msgSrcTask);
						sprintf(logPayloadBuff, "ERROR from source %d", read_queue->msgSrcTask); 
                        			//send_log_main(logPayloadBuff, LOG_ERROR, mainTaskLogMsg);

						//raise(SIGINT); /* TBD LED */ 
					}
                    else if(read_queue->msgId == TIVA_MSGID_ERROR)
                    {
                        printf("main::Received error from TIVA\n");
                        //LED glow
                        sprintf(logPayloadBuff, "ERROR from TIVA ");
                        send_log_main(logPayloadBuff, LOG_ERROR, mainTaskLogMsg);
                    }                    								
				}
			}while(n != EAGAIN);
			n = 0;
		}
	}

	printf("main()::Terminating signal received\n");

	/* Free all the dynamic memory */
	free(read_queue);
	free(mainTaskLogMsg);

    if(pthread_join(socketTask, NULL) == 0)      /* Wait until Socket thread has completed execution*/
       printf("Socket task done\n");
    else
       printf("Socket task Error\n");  

    if(pthread_join(logTask, NULL) == 0)      /* Wait until log thread has completed execution*/
       printf("Log task done\n");
    else
       printf("Log task Error\n");   
 
    if(pthread_join(gestureTask, NULL) == 0)      /* Wait until gesture thread has completed execution*/
       printf("Gesture task done\n");
    else
       printf("Gesture task Error\n"); 

	/* Close the message queues */
    if (mq_close (qdes_mainTask) == -1) 
    {
    	printf("main(): Error closing main queue %d\n", errno);
    }	   
    if (mq_close (qdes_socketTask) == -1) 
    {
        printf("main(): Error closing socket queue %d\n", errno);
    }
 
    if (mq_close (qdes_logTask) == -1) 
    {
        printf("main(): Error closing log queue %d\n", errno);
    } 

	/* Unlink the message queues */    
    if (mq_unlink (MAIN_TASK_MQ_NAME) == -1) 
    {
        //printf("main(): Error unlinking main queue %d\n", errno);
    }  

    if (mq_unlink (SOCKET_TASK_MQ_NAME) == -1) 
    {
        //printf("main(): Error unlinking socket queue %d\n", errno);
    }  

    if (mq_unlink (LOG_TASK_MQ_NAME) == -1) 
    {
       // printf("ERROR No: %d Unable to unlink the log queue \n", errno);
    }
       

    printf("EXITED NORMALLY\n"); 
	return 0;
}

void send_log_main(char * msg, LOGGER_level mainLogLevel, msgStruct_t *mainMsgPacket)
{
    send_log(MAIN_TASK_ID, mainLogLevel, msg, mainMsgPacket, qdes_logTask);    
}