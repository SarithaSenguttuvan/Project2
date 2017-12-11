/*
 * accelerometer.h
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#ifndef ACCELEROMETERTIVA_H_
#define ACCELEROMETERTIVA_H_
#include "genericTiva.h"
/**
 * @brief
 *      This function is the task function for the accelerometer task
 * @return
 *      void
 */
void accelerometerTask(void *pvParameters);

void send_socket_acc(char * msg, tiva_msgStruct_t *socketPacket);

#endif /* ACCELEROMETERTIVA_H_ */
