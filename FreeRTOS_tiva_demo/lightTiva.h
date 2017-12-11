/*
 * lightTiva.h
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#ifndef LIGHTTIVA_H_
#define LIGHTTIVA_H_
#include "genericTiva.h"

/**
 * @brief
 *      This function is the task function for the light task
 * @return
 *      void
 */
void lightTask(void *pvParameters);

void send_socket_light(char * msg, tiva_msgStruct_t *socketPacket);



#endif /* LIGHTTIVA_H_ */
