/** NG
 * @file 
 *		main.h 
 * @brief 
 *		This file contains function declarations for variables used in main.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#ifndef INCLUDES_MAIN_TASK_H_
#define INCLUDES_MAIN_TASK_H_

#include <pthread.h>
#include "log.h"
#include "generic.h"

/* Thread names */
pthread_t socketTask;
pthread_t decisionTask;
pthread_t logTask;
pthread_t gestureTask;

/* Thread atributes */
pthread_attr_t socketTask_attr;
pthread_attr_t decisionTask_attr;
pthread_attr_t logTask_attr;
pthread_attr_t gestureTask_attr;



/* Signal event structures */
struct sigevent mq_mainTask_timer;
struct sigevent mq_mainTask_notify;

/**
 * @brief 
 * 		This function is used to send logs to the log queue from main
 * @return 
 *      uint8_t
 */
void send_log_main(char * msg, LOGGER_level mainLogLevel, log_t *mainlogPacket, msgStruct_t *mainMsgPacket);

#endif
