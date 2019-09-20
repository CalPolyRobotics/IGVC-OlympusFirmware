/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of and a contribution to the lwIP TCP/IP stack.
 *
 * Credits go to Adam Dunkels (and the current maintainers) of this software.
 *
 * Christiaan Simons rewrote this file to get a more stable echo application.
 *
 **/

 /* This file was modified by ST */
 /* This file was modified by CalPolyRobotics */


#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"

#include "tcp_server.h"
#include "comms.h"

#if LWIP_TCP

#define TCP_SERVER_PORT 7

static struct tcp_pcb *server_pcb;

/* Connection states */
enum tcp_conn_states
{
  SRV_NONE = 0,
  SRV_CONNECTED,
};


/* TODO - should run tcp_recved after processing the incoming data */

/* structure for maintaing connection infos to be passed as argument
   to LwIP callbacks*/
typedef struct {
  u8_t state;             /* current connection state */
  CommsHandler_t *hdl;
}tcp_conn_t;


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_server_error(void *arg, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_server_connection_close(struct tcp_pcb *tpcb, tcp_conn_t *conn);
static void tcp_server_connection_free(tcp_conn_t *conn);

/**
  * @brief  Initializes the tcp server
  * @param  None
  * @retval None
  */
void tcp_server_init(void) {
  /* create new tcp pcb */
  server_pcb = tcp_new();

  if (server_pcb != NULL) {
    /* bind echo_pcb to TCP_SERVER_PORT */
    err_t err = tcp_bind(server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);

    if (err == ERR_OK) {
      /* start tcp listening for echo_pcb */
      server_pcb = tcp_listen(server_pcb);

      /* initialize LwIP tcp_accept callback function */
      tcp_accept(server_pcb, tcp_server_accept);
    } else {
      /* deallocate the pcb */
      memp_free(MEMP_TCP_PCB, server_pcb);
    }
  }
}

/**
  * @brief  This function is the implementation of tcp_accept LwIP callback
  * @param  arg: not used
  * @param  newpcb: pointer on tcp_pcb struct for the newly created tcp connection
  * @param  err: not used
  * @retval err_t: error status
  */
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  tcp_conn_t *conn = (tcp_conn_t *)mem_malloc(sizeof(tcp_conn_t));
  if(conn == NULL ){
    goto err_mem;
  }

  CommsHandler_t *hdl = (CommsHandler_t *)mem_malloc(sizeof(CommsHandler_t));
  if(hdl == NULL){
    mem_free(conn);
    goto err_mem;
  }

  initCommsHandler(hdl, newpcb);

  conn->state = SRV_CONNECTED;

  /* pass newly allocated es structure as argument to newpcb */
  tcp_arg(newpcb, conn);

  /* initialize lwip tcp_recv callback function for newpcb  */
  tcp_recv(newpcb, tcp_server_recv);

  /* initialize LwIP tcp_sent callback function */
  tcp_sent(newpcb, tcp_server_sent);

  /* initialize lwip tcp_err callback function for newpcb  */
  tcp_err(newpcb, tcp_server_error);

  /* initialize lwip tcp_poll callback function for newpcb */
  tcp_poll(newpcb, tcp_server_poll, 0);

  return ERR_OK;

err_mem:
  /*  close tcp connection */
  tcp_server_connection_close(newpcb, conn);

  /* return memory error */
  return ERR_MEM;
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {

  LWIP_ASSERT("arg != NULL",arg != NULL);

  tcp_conn_t *conn = (tcp_conn_t *)arg;

  if (err != ERR_OK) {
    /* free received pbuf*/
    pbuf_free(p);

    return err;
  }

  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL) {
    /*  close tcp connection */
    tcp_server_connection_close(tpcb, conn);

    return ERR_OK;
  }

  struct pbuf *curp = p;

  /* Loop till end condition of pbuf chain (totLen == len) */
  while(curp != NULL && curp->tot_len != curp->len){
    runCommsFSM(conn->hdl, (char *)(curp->payload), curp->len);
    tcp_recved(tpcb, curp->len);

    curp = curp->next;
  }

  /** This will free the entire chain */
  pbuf_free(p);

  return ERR_OK;
}

/**
  * @brief  This function implements the tcp_err callback function (called
  *         when a fatal tcp_connection error occurs.
  * @param  arg: pointer on argument parameter
  * @param  err: not used
  * @retval None
  */
static void tcp_server_error(void *arg, err_t err)
{
  LWIP_UNUSED_ARG(err);

  tcp_conn_t *conn = (tcp_conn_t *)arg;
  if (conn != NULL)
  {
    /*  free conn structure */
    tcp_server_connection_free(conn);
  }
}

/**
  * @brief  This function implements the tcp_poll LwIP callback function
  * @param  arg: pointer on argument passed to callback
  * @param  tpcb: pointer on the tcp_pcb for the current tcp connection
  * @retval err_t: error code
  */
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb)
{
  /** TODO - Nothing to do **/
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(tpcb);

  return ERR_OK;
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data)
  * @param  None
  * @retval None
  */
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  /** TODO - Currently does nothing **/

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(len);
  LWIP_UNUSED_ARG(tpcb);

  return ERR_OK;
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  conn: pointer on connection structure
  * @retval None
  */
static void tcp_server_connection_close(struct tcp_pcb *tpcb, tcp_conn_t *conn) {

  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);

  /* delete conn structure */
  if (conn != NULL) {
    tcp_server_connection_free(conn);
  }

  /* close tcp connection */
  tcp_close(tpcb);
}

/**
  * @brief  This functions frees the connection object
  * @param  conn: the connection struct for the current connection
  * @retval None
  */
static void tcp_server_connection_free(tcp_conn_t *conn){
    mem_free(conn->hdl);
    mem_free(conn);
}

/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  conn: pointer on server_handler structure
  * @retval None
  */
void tcp_server_send(struct tcp_pcb *tpcb, char *data, size_t len) {

  if(len <= tcp_sndbuf(tpcb)){
    /* enqueue data for transmission */
    err_t wr_err = tcp_write(tpcb, data, len, TCP_WRITE_FLAG_COPY);

    if (wr_err != ERR_OK) {
      printf("Failed to send ethernet data of size %d {failed write)\r\n", len);
    }

  }else{
    //TODO - queue data in this event
    printf("Failed to send ethernet data of size %d (sndbuf full)\r\n", len);
  }
}

#endif /* LWIP_TCP */
