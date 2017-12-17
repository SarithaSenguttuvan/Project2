/*
 * mainTask.h
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#ifndef MAINTIVA_H_
#define MAINTIVA_H_

#include "timers.h"
/**
 * @brief
 *      This function is the task function for the main task
 * @return
 *      void
 */
void mainTask(void *pvParameters);

/**
 * @brief
 *      This function is the timer callback
 * @return
 *      void
 */
static void myTimerCallBack(TimerHandle_t xTimer );


void send_socket_main(char * msg, tiva_msgStruct_t *mainPacket, tiva_msgid_t mainMsgID, bool isr);


#endif /* MAINTIVA_H_ */
