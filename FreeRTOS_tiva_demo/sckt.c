/*
 * sckt.c
 *
 *  Created on: Dec 6, 2017
 *      Author: Saritha
 */
#if 0
#include "genericTiva.h"

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
//ip_addr_t *ip = NULL;

//char charIP[4] = {10,0,0,104};
uint32_t g_ui32IPAddress;
//uint32_t ip = 0x6800000A;   //10.0.0.104
struct ip_addr testIP = { 0x6800000A };

ip_addr_t ipAddr = { 0x6800000A };
extern uint32_t output_clock_rate_hz;

char sendPacket[BUFFER_LENGTH];
tiva_msgStruct_t msgPacket;//(tiva_msgStruct_t *)malloc(sizeof(tiva_msgStruct_t));

struct tcp_pcb *tcp_echoserver_pcb;

#define SYSTICK_INT_PRIORITY    0x80
#define ETHERNET_INT_PRIORITY   0xC0

#define TIVA_SCKT_ADDR  "192.168.1.1"
#define POLLING_GAP        4

extern bool TimerFlag;
bool IpFlag = false;
bool globalScktFlag = false;

void clientSckt_task(void *pvParameters)
{
    uint32_t ui32User0, ui32User1;
    uint8_t pui8MACArray[8];
    uint32_t socketTaskNotificationValue = 0;
    //ipaddr.addr = ip;
    //ip = (ip_addr_t*)charIP;
    //struct sockaddr_in addr;

       MAP_FlashUserGet(&ui32User0, &ui32User1);
       if((ui32User0 == 0xffffffff) || (ui32User1 == 0xffffffff))
       {
           //
           // We should never get here.  This is an error if the MAC address has
           // not been programmed into the device.  Exit the program.
           // Let the user know there is no MAC address
           //
           UARTprintf("No MAC programmed!\n");
           while(1)
           {
           }
       }

       //
       // Tell the user what we are doing just now.
       //
       UARTprintf("Waiting for IP.\n");

       //
       // Convert the 24/24 split MAC address from NV ram into a 32/16 split MAC
       // address needed to program the hardware registers, then program the MAC
       // address into the Ethernet Controller registers.
       //
       pui8MACArray[0] = ((ui32User0 >>  0) & 0xff);
       pui8MACArray[1] = ((ui32User0 >>  8) & 0xff);
       pui8MACArray[2] = ((ui32User0 >> 16) & 0xff);
       pui8MACArray[3] = ((ui32User1 >>  0) & 0xff);
       pui8MACArray[4] = ((ui32User1 >>  8) & 0xff);
       pui8MACArray[5] = ((ui32User1 >> 16) & 0xff);

       lwIPInit(output_clock_rate_hz, pui8MACArray, 0, 0, 0, IPADDR_USE_DHCP);
       vTaskDelay(pdMS_TO_TICKS(2000));
       //
       // Setup the device locator service.
       //
       LocatorInit();
       LocatorMACAddrSet(pui8MACArray);
       LocatorAppTitleSet("AES - Project2");


       MAP_IntPrioritySet(INT_EMAC0, ETHERNET_INT_PRIORITY);
       MAP_IntPrioritySet(FAULT_SYSTICK, SYSTICK_INT_PRIORITY);

       //
      // Initialize a TCP connection.
      //
      my_tcp_init();
       /* tcp echo server Init */
       //tcp_echoserver_init();
       //while(1);
      TimerFlag = true;
      while(1)
      {
          if(xTaskNotifyWait( 0, ULONG_MAX, &socketTaskNotificationValue, portMAX_DELAY) != pdFALSE)
          {
              if(socketTaskNotificationValue & 4)
              {
                  if(globalScktFlag)
                  {
                      tcp_write(tcp_echoserver_pcb,"Raj",3,TCP_WRITE_FLAG_COPY);
                      tcp_output(tcp_echoserver_pcb);
                  }
              }
          }
      }
}

void my_tcp_init()
{
    //struct tcp_pcb *tcp_echoserver_pcb;
    err_t err;

    ip_addr_t myip_addr = { (u32_t)1090519050 };

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

            //
                       // Toggle the GPIO
                       //
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
    uint16_t len = 0;   /*length of bytes read*/
    len = p->len;
   // char *recvData = (char *) malloc(len);

    UARTprintf("\r\n my_recCallBack():: Data Received is %s %d", (char *)p->payload, len);

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
#endif
