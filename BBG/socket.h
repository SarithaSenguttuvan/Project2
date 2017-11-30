/** NG
 * @file 
 *		socket.h 
 * @brief 
 *		This file contains function declarations for variables used in socket.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#ifndef INCLUDES_SOCKET_TASK_H_
#define INCLUDES_SOCKET_TASK_H_

#include "log.h"
#include "generic.h"
/**
 * @brief 
 * 		This function is the pthread function for the Socket task
 * @return 
 *      void
 */
void *socketTaskFunc(void *arg);

/**
 * @brief 
 * 		This function is used to send logs to the log queue from socket task
 * @return 
 *      uint8_t
 */
void send_log_socket(char * msg, LOGGER_level socketLogLevel, log_t *socketlogPacket, msgStruct_t *socketMsgPacket);


#endif
