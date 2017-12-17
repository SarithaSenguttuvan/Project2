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

#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define PORT (8000)
#define TIVA_DELIMITER ("$")
extern bool sigHandle;
mqd_t socket_qdes_log;
	
/* Start routine for the Socket Task */
void *socketTaskFunc(void *arg)
{
	char dummyAmbience[20] = "Day";
	int8_t rc = 0;	
	int8_t n = 0;	
	int32_t recvSig = 0;

	/* Sockets */ 
	//int bbg_serv_sd;
	int bbg_serv_fd, conn_socket;
	int ret = 0;
	int socket_numbytes = 0;
	struct sockaddr_in serv_addr, bbg_addr, usr_add;
	int flags = 0;
	bool connection = false;
	char buffer[1024] = {0};

	mqd_t socket_qdes_main;
	mqd_t socket_qdes_socket;
	mqd_t socket_qdes_decision;
	struct sigevent mq_socketTask_notify; 

	msgStruct_t *read_socket_msg_queue = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *HB_socket = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *socketTaskLogMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t));
	msgStruct_t *TivaMsg = (msgStruct_t *)malloc(sizeof(msgStruct_t)); 
	msgStruct_t rec_msg;
	rec_msg.logLevel = LOG_NONE; 

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
#if 0
	if((socket_qdes_decision = mq_open(DECISION_TASK_MQ_NAME, O_NONBLOCK | O_WRONLY)) == (mqd_t)-1)  /* open the decision task queue */
	{
    	printf("socket()::decision queue open error is %d\n", errno);
    }
#endif
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

    if ((bbg_serv_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)       /* Create a socket on the client side */
    {
        printf("\n Socket creation error \n");
    }

	bbg_addr.sin_family = AF_INET;
	bbg_addr.sin_addr.s_addr = INADDR_ANY;
	bbg_addr.sin_port = htons( PORT );

	/* Bind address */
	if (bind(bbg_serv_fd, (struct sockaddr *)&bbg_addr, sizeof(bbg_addr))<0) 
	{
	    printf("error in binding socket\n");
	}

	/* Listening on socket */
	if (listen(bbg_serv_fd, 7) < 0)       
	{
	    printf("error in listening socket\n");
	}
#if 1
	 /* Set socket to non-blocking */ 
	if ((flags = fcntl(bbg_serv_fd, F_GETFL, 0)) < 0) 
	{ 
	    printf("Socket():: Error getting the socket flags\n");
	} 
	if (fcntl(bbg_serv_fd, F_SETFL, flags | O_NONBLOCK) < 0) 
	{ 
	    printf("Socket():: Error setting the socket flags\n");
	}
#endif
	printf("Socket(): Before while\n");
	while(!sigHandle)
	{
		recvSig = unblockOnSignal(SOCKET_TASK_ID); 		/* unblock on SIGSOCKET */ 
		
		if(recvSig == SIGHBSOCKET)
		{
			// *TBD* send heartbeat
			printf("socket()::Received HB req\n");     	 	//Remove  
			send_heartBeat(SOCKET_TASK_ID,HB_socket, socket_qdes_main);
			//connection = true; //Remove
			#if 0
			if(connection == true)
			{
				//ret = read( bbg_serv_sd , buffer, 1024);	//Remove comment
				if(ret == -1)
        			printf("socket():: Error %d in reading from socket\n", errno); 

        		char *msgField = strtok(buffer, TIVA_DELIMITER);
        		printf("msgField is %s\n",msgField);
			    while(msgField != NULL)
			    {
			    	#if 0
			        printf("%s\n", msgField);
			        //sscanf(msgField, "%d ", &rec_msg.tivaMsgId);
    				//printf("The received msgId is %d\n", rec_msg.tivaMsgId);
		            if(rec_msg.tivaMsgId == TIVA_MSGID_ACCELEROMETER_DATA)
				    {
				        printf("First\n");
				        //sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, msg.tivaMsgPayload);
				        //rec_msg.tivaMsgPayload = NULL;
				        //SEND THE ENTIRE MSG TO MAIN TASK OR LOG					        
				    }
				    else if(rec_msg.tivaMsgId == TIVA_MSID_ACC_ALERT)
				    {
				        printf("Second\n");
				        //sscanf(msgField, "%d %d %ld %d %s", &rec_msg.tivaMsgId, &rec_msg.tivaMsgSrcTask, &rec_msg.tivaMsgPayloadLen, &rec_msg.tivadata, rec_msg.tivaMsgPayload);
				        //rec_msg.tivaMsgPayload = NULL;
				        printf("Second\n");
				        //rec_msg.tivadata = 0;
				    }
				    #endif
				    printf("msgField is %s\n",msgField);
			    	if(!(strcmp(msgField, "light")))
			    	{
			    		//send light alert
			    		makeMsg(TivaMsg, TIVA_MSGID_LIGHT_ALERT, "Ambience changed", LOG_WARN);
			    		send_log_socket("Ambience changed", LOG_WARN, socketTaskLogMsg);
			    	}
			    	else if(!(strcmp(msgField, "accelerometer")))
			    	{
			    		//send acc alert
			    		makeMsg(TivaMsg, TIVA_MSGID_ACC_ALERT, "Door opened", LOG_WARN);
					printf("Accelerometer*******\n");
			    		send_log_socket("Door opened", LOG_WARN, socketTaskLogMsg);			    		
			    	}
			    	else if(!(strcmp(msgField, "LightSensorInitialized")))
			    	{
			    		//send acc alert
			    		makeMsg(TivaMsg, TIVA_MSGID_INITIALIZED, "I2C sensor Initialized", LOG_INFO);
			    		send_log_socket("LightSensorInitialized", LOG_INFO, socketTaskLogMsg);			    		
			    	}
			    	else if(!(strcmp(msgField, "AccSensorInitialized")))
			    	{
			    		//send acc alert
			    		makeMsg(TivaMsg, TIVA_MSGID_INITIALIZED, "I2C sensor Initialized", LOG_INFO);
			    		send_log_socket("AccSensorInitialized", LOG_INFO, socketTaskLogMsg);			    		
			    	}			    				    	
			    	else if(!(strcmp(msgField, "error")))
			    	{
			    		//send err alert
			    		makeMsg(TivaMsg, TIVA_MSGID_ERROR, "Tiva error", LOG_ERROR);
			    		send_log_socket("Tiva error", LOG_ERROR, socketTaskLogMsg);	
						if(mq_send(socket_qdes_main, (char *)TivaMsg, sizeof(msgStruct_t), 0) == -1) 		/* Send heartbeat to main task */
					    {
					    	printf ("sendTivaError::Error no %d\n", errno);
					    }
					    else
					    	printf("sendTivaError::Error sent\n"); 
			     	}			    	
				    msgField = strtok(NULL, TIVA_DELIMITER);
			    }
			}
			#endif
			if(!connection)
			{
				printf("*********************************Waiting1\n");
				conn_socket = accept(bbg_serv_fd, (struct sockaddr *)&usr_add, (socklen_t*)&usr_add); //returns 
				if(conn_socket < 0) 
				{
					if (errno == EAGAIN) 
					{
					    connection = false;
					    printf("No incoming connections received\n");
					}
					else
					{
						printf("Socket():: Error in socket accept\n");
					}
				}
				else
				{
					connection = true;
					printf("*********************************Waiting2 connection made\n");
				}			

			}
			else
			{
				printf("*********************************Waiting3 Reception\n");
				socket_numbytes = recv(conn_socket, buffer , 1024 , MSG_DONTWAIT);
				printf("*********************************Waiting4 After Reception\n");

				char *msgField = strtok(buffer, TIVA_DELIMITER);


		        if(socket_numbytes < 0) //Error
		        {
		        	printf("*********************************Waiting6 Error\n");
		            if(errno == EAGAIN)
		            {
		                printf("Connection is alive! No msg received\n");
		            }
		            else
		            {
		                perror("Recv error\n");    
		            }
		        }
		        else if(socket_numbytes == 0)
		        {
		            printf("*********************************Waiting7 0\n");
		            printf("Connection Invalid\n");
		            //connection = false;
		        }
		        else 
		        {
					printf("*********************************Waiting5 Reading\n");
					while(msgField != NULL)
				    {
		            	printf("%s@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n",msgField );
		            	send_log_socket(buffer , LOG_INFO, socketTaskLogMsg);
		            	msgField = strtok(NULL, TIVA_DELIMITER);
				    }       	
		        }
			}
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
					if(read_socket_msg_queue->msgId == MSGID_PROX_NOTIFY)		/* If proximity data */										
					{		
						//send it through socket
						printf("socket()::Received from gesture to send on socket\n");
						if((send(conn_socket , "bbgData" , strlen("bbgData"), 0 )) == -1)
							printf("socket():: $$$$$$$$$$$$$$$$$$Error\n"); 
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

	/* close all the message queues */
    if (mq_close (socket_qdes_main) == -1) 
    {
        printf("socket()::error no %d closing main queue\n",errno);
    }
    if (mq_close (socket_qdes_socket) == -1) 
    {
        printf("socket()::error no %d closing socket queue\n",errno);
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
//close(conn_socket);
	
	pthread_exit(NULL);
}

void send_log_socket(char * msg, LOGGER_level socketLogLevel, msgStruct_t *socketMsgPacket)
{
    send_log(SOCKET_TASK_ID, socketLogLevel, msg, socketMsgPacket, socket_qdes_log);    
}

void makeMsg(msgStruct_t *socketMsgPacket, msgid_t msgID, char* msg, LOGGER_level levelLog)
{
	socketMsgPacket->msgId = msgID;
	socketMsgPacket->msgSrcTask = SOCKET_TASK_ID;
	socketMsgPacket->msgPayload = msg;
	socketMsgPacket->msgPayloadLen = strlen(msg);
	socketMsgPacket->logLevel = levelLog;
	socketMsgPacket->int_data = 0;
}