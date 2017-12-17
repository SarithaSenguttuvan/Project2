/**
 * @file 
 *		client.c
 * @brief 
 *		This file from my PC is used to communicate to the BBG application using socket programming
 * @author 
 *		Saritha Senguttuvan
 * @date 
 *		November 19,2017
 *
 */
 
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h> 
#include <stdbool.h>
#include <errno.h>

#define PORT 8000
#define BUFFER_LENGTH 256   
  
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, ret;
    struct sockaddr_in serv_addr;
    
    char buffer[1024] = {0};
    int LED_period = 5000;
    int LED_dutyCycle = 2000;
    bool LED_Toggle = false;
    int read_query = 2;
    char stringFromClient[BUFFER_LENGTH];
    
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)       /* Create a socket on the client side */
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    /* Initialize the server address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "10.0.0.169", &serv_addr.sin_addr)<=0) /* convert to network byte order */
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    getchar();

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) /* Connect to the server socket */
    {
        printf("\nConnection Failed \n");
	printf("Error number is %d\n", errno);
        return -1;
    }
	else
		printf("client_bbg():: Connection success\n");
	
    sprintf(stringFromClient, "%d %d %d %d\0", LED_period,LED_dutyCycle, LED_Toggle, read_query);
    //
    //
    //if((send(sock , stringFromClient , strlen(stringFromClient) , 0 )) == -1)		 /* Send to the BBG user application using sockets */

//	printf("client_bbg():: send ERROR\n");
//	else
//		printf("client_bbg():: send Success\n");
	//while(1)
	
while(1)
{	
	ret = read( sock , buffer, 10);
    if(ret == -1)
        printf("ERROR: %d in reading from client\n", errno); 
    printf("%s\n",buffer );
}
    //send(sock,stringFromClient,strlen(stringFromClient),0);
    //while(1);
    return 0;
}
