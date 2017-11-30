/** NG
 * @file 
 *		gesture.h 
 * @brief 
 *		This file contains function declarations for variables used in gesture.c
 *		 
 * @author 
 *		Saritha Senguttuvan, Savitha Senguttuvan
 * @date 
 *		December 12,2017
 *
 */

#ifndef INCLUDES_GESTURE_TASK_H_
#define INCLUDES_GESTURE_TASK_H_

#include "log.h"
#include "generic.h"

/**
 * @brief 
 * 		This function is the pthread function for the Gesture task
 * @return 
 *      void
 */
void *gestureTaskFunc(void *arg);


/**
 * @brief 
 * 		This function is used to send logs to the log queue from gesture task
 * @return 
 *      uint8_t
 */
void send_log_gesture(char * msg, LOGGER_level gestureLogLevel, log_t *gesturelogPacket, msgStruct_t *gestureMsgPacket);

#endif
