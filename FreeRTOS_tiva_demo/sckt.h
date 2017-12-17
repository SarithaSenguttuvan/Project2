/*
 * sckt.h
 *
 *  Created on: Dec 6, 2017
 *      Author: Saritha
 */

#ifndef SCKT_H_
#define SCKT_H_

#include "lwip/err.h"
#include "lwip/tcp.h"

#define SOCKET_PRIORITY TCP_PRIO_MIN

void clientSckt_task(void *pvParameters);

void my_tcp_init(void);

void DisplayIPAddress(uint32_t ui32Addr);

//static err_t *tcp_ser_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
err_t tcp_ser_accept(void *arg, struct tcp_pcb *newpcb, err_t ret_err);

err_t my_CallBack(void *arg, struct tcp_pcb *tpcb, err_t err);

err_t my_recCallBack(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

static void my_err(void *arg, err_t err);

static err_t my_sent(void *arg, struct tcp_pcb *pcb, u16_t len);

static err_t my_poll(void *arg, struct tcp_pcb *pcb);

void sendData(char *inputBufWrite, tiva_msgStruct_t *msgPacket, struct tcp_pcb *newpcb);

tcp_connected_fn myCallBAck(void *arg, struct tcp_pcb *tpcb, err_t err);

#endif /* SCKT_H_ */
