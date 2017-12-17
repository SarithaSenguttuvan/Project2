/*
 * socketTiva.c
 *
 *  Created on: Nov 29, 2017
 *      Author: Saritha
 */

#include "socketTiva.h"
#include "genericTiva.h"

//From sckt.c

#include "sckt.h"
#include "utils/lwiplib.h"
#include "utils/locator.h"
#include "utils/ustdlib.h"
#include "inc/hw_ints.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/opt.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define SYSTICKMS 10
#define BUFFER_LENGTH 1024

uint32_t g_ui32IPAddress;
struct ip_addr testIP = { 0x6800000A };

ip_addr_t ipAddr = { 0x6800000A };
extern uint32_t output_clock_rate_hz;

char sendPacket[BUFFER_LENGTH];
tiva_msgStruct_t msgPacket;

struct tcp_pcb *tcp_echoserver_pcb;

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

#define TIVA_SCKT_ADDR  "192.168.1.1"
#define POLLING_GAP        4

extern bool TimerFlag;
bool IpFlag = false;
bool globalScktFlag = false;


//Queue handles
extern QueueHandle_t xMainQueue;
extern QueueHandle_t xSocketQueue;
extern QueueHandle_t xLightQueue;
extern QueueHandle_t xAccelerometerQueue;

#define SOCKET_TASK_WAIT_TIME   pdMS_TO_TICKS( 2000 )
#define LIGHT_SOCKET ("light$")
#define ACC_SOCKET ("accelerometer$")
#define ERR_SOCKET ("error$")

// Function for socket task
void socketTask(void *pvParameters)
{
    //BaseType_t xSocketStatus;
    size_t socketMsgCount = 0;
    BaseType_t xSocketStatus;
    tiva_msgStruct_t socketHBMsg;
    uint32_t socketNotificationValue = 0;
    tiva_msgStruct_t socketReceivedmsg;

    UARTprintf("\r\n In the Socket task");

    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];
   // uint32_t socketTaskNotificationValue = 0;

    MAP_FlashUserGet(&ui32User0, &ui32User1);
    if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
    {
        UARTprintf("No MAC programmed!\n");
        while(1)
        {
        }
    }

    UARTprintf("Waiting for IP.\n");

    pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
    pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
    pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
    pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
    pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
    pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

    lwIPInit(output_clock_rate_hz, pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);
    vTaskDelay(pdMS_TO_TICKS(2000));

    LocatorInit();
    LocatorMACAddrSet(pui8MACArray);
    LocatorAppTitleSet("AES - Project2");

    MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
    MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

   my_tcp_init();
   TimerFlag = true;

    for (;;)
    {
        if(xTaskNotifyWait( 0, ULONG_MAX, &socketNotificationValue, portMAX_DELAY) != pdFALSE)
        {
            UARTprintf("\r\n socket():: !!!!!!!!!Received Timer notification");
            if((socketNotificationValue & HB_REQ_BIT))
            {
                sendHB(TIVA_SOCKET_TASK_ID, &socketHBMsg);
                UARTprintf("\r\n Socket():: ?????Received HB request");
                //*TBD* send HB
                //xSocketStatus = xQueueSendToBack( xMainQueue, &socketHBMsg, 0 );
            }
            if(socketNotificationValue & 4)
            {
                if(globalScktFlag)
                {
                    //tcp_write(tcp_echoserver_pcb,"Raj",3,TCP_WRITE_FLAG_COPY);
                    //tcp_output(tcp_echoserver_pcb);
                }
            }
            if((socketNotificationValue & MQ_RECEIVE_BIT))
            {
                while((socketMsgCount = uxQueueMessagesWaiting(xSocketQueue)) != 0)
                {
                    xSocketStatus = xQueueReceive( xSocketQueue, (tiva_msgStruct_t *)&socketReceivedmsg, 0 );
                    if( xSocketStatus == pdPASS )
                    {
                        //*TBD* send logs
                        UARTprintf("\r\n socket():: xQueueReceive Success");
                    }
                    else
                    {
                        //*TBD* send logs
                        UARTprintf("\r\n Socket():: xQueueReceive Failure");
                    }
                    if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_LIGHT_DATA)
                    {
                        //Send to socket ('A') send(LIGHT_SOCKET);
                        if(globalScktFlag)
                        {
                            tcp_write(tcp_echoserver_pcb,"AmbienceChanged$",16,TCP_WRITE_FLAG_COPY);
                            tcp_output(tcp_echoserver_pcb);
                        }
                    }
                    else if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_ACCELEROMETER_DATA)
                    {
                        //Send to socket ('B') send(ACC_SOCKET);
                        if(globalScktFlag)
                        {
                            tcp_write(tcp_echoserver_pcb,"accelerometer$",14,TCP_WRITE_FLAG_COPY);
                            tcp_output(tcp_echoserver_pcb);
                        }
                    }
                    else if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_INIT)
                    {
                        //Send to socket ('B') send(ACC_SOCKET);
                        if(globalScktFlag)
                        {
                            tcp_write(tcp_echoserver_pcb,"TivaInitialized$",16,TCP_WRITE_FLAG_COPY);
                            tcp_output(tcp_echoserver_pcb);
                        }
                    }
                    else if(socketReceivedmsg.tivaMsgId == TIVA_MSGID_ERROR)
                    {
                        //Send to socket ('C') send(ERR_SOCKET);
                        if(globalScktFlag)
                        {
                            tcp_write(tcp_echoserver_pcb,"error$",6,TCP_WRITE_FLAG_COPY);
                            tcp_output(tcp_echoserver_pcb);
                        }
                    }
                }
                UARTprintf("\r\n Socket():: ?????Received HB request");
            }
        }
        else
        {
            UARTprintf("\r\n Socket()::xTaskNotifyWait error");
        }
        //xReturn = ( BaseType_t ) ulTaskNotifyTake( pdTRUE, pxUARTInstance->xTxTimeout );
    }
}

void my_tcp_init()
{
    //struct tcp_pcb *tcp_echoserver_pcb;
    err_t err;

    //ip_addr_t myip_addr = { (u32_t)1090519050 };
    ip_addr_t myip_addr = { (u32_t)3230509248 };

    /* create new tcp pcb */
    tcp_echoserver_pcb = tcp_new();
    if (tcp_echoserver_pcb != NULL)
    {
        UARTprintf("tcp_new() Success\n");

    }
    else
    {
       UARTprintf("tcp_new() Error\n");
    }

    tcp_setprio(tcp_echoserver_pcb, SOCKET_PRIORITY);
#if 0
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, 8000);
    if (err != ERR_OK)
    {
        UARTprintf("my_tcp_init():: Error binding\n");
    }
    else
    {

    }

    /* start tcp listening for echo_pcb */
    tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
    if(tcp_echoserver_pcb == NULL)
    {
        UARTprintf("my_tcp_init():: tcp_listen Error\n");
    }

    tcp_arg(tcp_echoserver_pcb, tcp_echoserver_pcb);

    tcp_accept(tcp_echoserver_pcb, tcp_ser_accept);         /* initialize LwIP tcp_accept callback function */
#endif

    while(!IpFlag);

    /* Make TCP connect */
    if((tcp_connect(tcp_echoserver_pcb, &myip_addr , 8000, myCallBAck(NULL,tcp_echoserver_pcb,ERR_OK))) == ERR_OK)
    {
        UARTprintf("my_tcp_init():: Success in tcp_connect\n");
        tcp_recv(tcp_echoserver_pcb, my_recCallBack);
        tcp_sent(tcp_echoserver_pcb, my_sent);
        globalScktFlag = true;
    }
}

void DisplayIPAddress(uint32_t ui32Addr)
{
    char pcBuf[16];
    UARTprintf("\r\n IP address is");
    usprintf(pcBuf, "%d.%d.%d.%d", ui32Addr & 0xff, (ui32Addr >> 8) & 0xff,(ui32Addr >> 16) & 0xff, (ui32Addr >> 24) & 0xff); /* Convert the IP Address into a string. */
    UARTprintf(pcBuf); /* Display the string */
}

void lwIPHostTimerHandler(void)
{
    uint32_t ui32Idx, ui32NewIPAddress;
    ui32NewIPAddress = lwIPLocalIPAddrGet();    /* Get the current IP address */
    if(ui32NewIPAddress != g_ui32IPAddress) /* See if the IP address has changed */
    {
        if(ui32NewIPAddress == 0xffffffff)  /* See if there is an IP address assigned */
        {
            UARTprintf("Waiting for link.\n");  /* Indicate that there is no link */
        }
        else if(ui32NewIPAddress == 0)
        {
            UARTprintf("Waiting for IP address.\n");    /* There is no IP address, so indicate that the DHCP process is running */
        }
        else
        {
            UARTprintf("TIVA IP Address: ");    /* Display the new IP address */
            IpFlag = true;
            DisplayIPAddress(ui32NewIPAddress);
        }
        g_ui32IPAddress = ui32NewIPAddress;    /* Save the new IP address */
    }

    if((ui32NewIPAddress == 0) || (ui32NewIPAddress == 0xffffffff))     /* If there is not an IP address */
    {
        for(ui32Idx = 1; ui32Idx < 17; ui32Idx++)
        {

                       MAP_GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,
                               (MAP_GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1) ^
                                GPIO_PIN_1));

                       SysCtlDelay(output_clock_rate_hz/(ui32Idx << 1));
        }
    }

}

err_t tcp_ser_accept(void *arg, struct tcp_pcb *newpcb, err_t ret_err)
{
    UARTprintf("\r\n tcp_ser_accept()::Reached accept callback");
    struct tcp_pcb_listen *listen_pcb = (struct tcp_pcb_listen*)arg;

    tcp_accepted(listen_pcb);

    tcp_setprio(newpcb, SOCKET_PRIORITY);

    tcp_arg(newpcb, newpcb);

    /* While registering for callback, it should not be in listen state */
    tcp_recv(newpcb, my_recCallBack);
    tcp_err(newpcb, my_err);
    tcp_poll(newpcb, my_poll, POLLING_GAP);
    tcp_sent(newpcb, my_sent);
    //tcp_write(newpcb,sendData(sendPacket),strlen(sendPacket),TCP_WRITE_FLAG_COPY);
    //sendData(sendPacket, &msgPacket, newpcb);
//    tcp_write(newpcb,"Saritha",7,TCP_WRITE_FLAG_COPY);
//    tcp_output(newpcb);
//    tcp_write(newpcb,"Savitha",7,TCP_WRITE_FLAG_COPY);
//    tcp_output(newpcb);
    globalScktFlag = true;
    return ERR_OK;
}

err_t my_CallBack(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    return ERR_OK;
}

err_t my_recCallBack(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
   // static int cnt = 0;
    uint16_t len = 0;   /*length of bytes read*/
    len = p->len;
   // char *recvData = (char *) malloc(len);

    //cnt++;
    UARTprintf("\r\n my_recCallBack():: Data Received is %s %d", (char *)p->payload, len);

    LEDWrite(0x0F, 0x04);


    //tcp_recved(tpcb,len);

    return ERR_OK;
}

void SysTickIntHandler(void)
{
    lwIPTimer(SYSTICKMS);
}

static void my_err(void *arg, err_t err)
{
  UARTprintf("\r\ntcp_err: %s", "ERROR");
  //globalScktFlag = false;
}
static err_t my_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
   UARTprintf("\r\nData has been sent");
   return ERR_OK;
}

static err_t my_poll(void *arg, struct tcp_pcb *pcb)
{
    err_t err;
    //UARTprintf("\r\nConnection polling");
//    tcp_arg(pcb, NULL);
//    tcp_recv(pcb, NULL);
//    tcp_err(pcb, NULL);
//    tcp_poll(pcb, NULL, 0);
    //tcp_sent(pcb, NULL);
//    err = tcp_close(pcb);
    return ERR_OK;
}

void sendData(char *inputBufWrite, tiva_msgStruct_t *msgPacket, struct tcp_pcb *newpcb)
{
    //sprintf(inputBufWrite, "%d %d %ld %d %s", msgPacket->tivaMsgId, msgPacket->tivaMsgSrcTask , msgPacket->tivaMsgPayloadLen , msgPacket->tivadata, msgPacket->tivaMsgPayload);
    sprintf(inputBufWrite, "%d %d %ld %d %s", 1, 2 , 3 , 4, "saritha");
    tcp_write(newpcb,inputBufWrite,strlen(inputBufWrite),TCP_WRITE_FLAG_COPY);
    tcp_output(newpcb);
    return;
}
tcp_connected_fn myCallBAck(void *arg, struct tcp_pcb *tpcb, err_t err)
{

}


