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

/* ECHO protocol states */
enum tcp_server_states
{
  SRV_NONE = 0,
  SRV_ACCEPTED,
  SRV_RECEIVED,
  SRV_CLOSING
};

/* TODO - should run tcp_recved after processing the incoming data */

/* structure for maintaing connection infos to be passed as argument 
   to LwIP callbacks*/
struct tcp_server_struct
{
  u8_t state;             /* current connection state */
  u8_t retries;
  struct tcp_pcb *pcb;    /* pointer on the current tcp_pcb */
  struct pbuf *p;         /* pointer on the received/to be transmitted pbuf */
};


static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_server_error(void *arg, err_t err);
static err_t tcp_server_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *srv);
static void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *srv);

/**
  * @brief  Initializes the tcp echo server
  * @param  None
  * @retval None
  */
void tcp_server_init(void) {
  /* create new tcp pcb */
  server_pcb = tcp_new();

  if (server_pcb != NULL) {
    err_t err;
    
    /* bind echo_pcb to TCP_SERVER_PORT */
    err = tcp_bind(server_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    
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
  err_t ret_err;
  struct tcp_server_struct *srv;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* set priority for the newly accepted tcp connection newpcb */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* allocate structure srv to maintain tcp connection informations */
  srv = (struct tcp_server_struct *)mem_malloc(sizeof(struct tcp_server_struct));
  if (srv != NULL) {

    srv->state = SRV_ACCEPTED;
    srv->pcb = newpcb;
    srv->retries = 0;
    srv->p = NULL;
    
    /* pass newly allocated es structure as argument to newpcb */
    tcp_arg(newpcb, srv);
    
    /* initialize lwip tcp_recv callback function for newpcb  */ 
    tcp_recv(newpcb, tcp_server_recv);
    
    /* initialize lwip tcp_err callback function for newpcb  */
    tcp_err(newpcb, tcp_server_error);
    
    /* initialize lwip tcp_poll callback function for newpcb */
    tcp_poll(newpcb, tcp_server_poll, 0);
    
    ret_err = ERR_OK;

  } else {
    /*  close tcp connection */
    tcp_server_connection_close(newpcb, srv);
    /* return memory error */
    ret_err = ERR_MEM;
  }

  return ret_err;  
}


/**
  * @brief  This function is the implementation for tcp_recv LwIP callback
  * @param  arg: pointer on a argument for the tcp_pcb connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  pbuf: pointer on the received pbuf
  * @param  err: error information regarding the reveived pbuf
  * @retval err_t: error code
  */
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb,struct pbuf *p, err_t err) {
  struct tcp_server_struct *srv;

  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  srv = (struct tcp_server_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL) {

    /* remote host closed connection */
    srv->state = SRV_CLOSING;
    if(srv->p == NULL) {
       /* we're done sending, close connection */
       tcp_server_connection_close(tpcb, srv);

    } else {
      /* we're not done yet */

      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_server_sent);

      /* send remaining data */
      tcp_server_send(tpcb, srv);
    }

    return ERR_OK;
  }

  if (err != ERR_OK) {
      /* free received pbuf*/
      srv->p = NULL;
      pbuf_free(p);

      return err;
  }


  switch(srv->state){
    case SRV_ACCEPTED:
      /* first data chunk in p->payload */
      srv->state = SRV_RECEIVED;
      
      /* store reference to incoming pbuf (chain) */
      srv->p = p;
      
      /* initialize LwIP tcp_sent callback function */
      tcp_sent(tpcb, tcp_server_sent);

      /* TODO - intercept data here - send back received data */
      tcp_server_send(tpcb, srv);
      
      break;

    case SRV_RECEIVED:
      /* more data received from client and previous data has been already sent*/
      if(srv->p == NULL) {
        srv->p = p;

        /* TODO - intercept here - send back received data */
        tcp_server_send(tpcb, srv);

      } else {
        struct pbuf *ptr;

        /* chain pbufs to the end of what we recv'ed previously  */
        ptr = srv->p;
        pbuf_chain(ptr,p);
      }

      break;

    default:
      /* Weird state, trash data  */
      tcp_recved(tpcb, p->tot_len);
      srv->p = NULL;
      pbuf_free(p);
      break;
  }

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
  struct tcp_server_struct *srv;

  LWIP_UNUSED_ARG(err);

  srv = (struct tcp_server_struct *)arg;
  if (srv != NULL)
  {
    /*  free srv structure */
    mem_free(srv);
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
  struct tcp_server_struct *srv;

  srv = (struct tcp_server_struct *)arg;
  if (srv != NULL) {
    if (srv->p != NULL) {
      tcp_sent(tpcb, tcp_server_sent);

      /* TODO - there is a remaining pbuf (chain), try to send data */
      tcp_server_send(tpcb, srv); 

    } else {
      /* no remaining pbuf (chain)  */
      if(srv->state == SRV_CLOSING) {
        /*  close tcp connection */
        tcp_server_connection_close(tpcb, srv);
      }
    }

    return ERR_OK;

  } else {
    /* nothing to be done */
    tcp_abort(tpcb);
    return ERR_ABRT;
  }
}

/**
  * @brief  This function implements the tcp_sent LwIP callback (called when ACK
  *         is received from remote host for sent data) 
  * @param  None
  * @retval None
  */
static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_server_struct *srv;

  LWIP_UNUSED_ARG(len);

  srv = (struct tcp_server_struct *)arg;
  srv->retries = 0;
  
  if(srv->p != NULL) {
    /* still got pbufs to send */
    tcp_sent(tpcb, tcp_server_sent);
    tcp_server_send(tpcb, srv);

  } else {
    /* if no more data to send and client closed connection*/
    if(srv->state == SRV_CLOSING) {
      tcp_server_connection_close(tpcb, srv);
    }
  }

  return ERR_OK;
}

/**
  * @brief  This function is used to send data for tcp connection
  * @param  tpcb: pointer on the tcp_pcb connection
  * @param  srv: pointer on server_handler structure
  * @retval None
  */
void tcp_server_send(struct tcp_pcb *tpcb, struct tcp_server_struct *srv) {
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (srv->p != NULL) && 
         (srv->p->len <= tcp_sndbuf(tpcb))) {
    
    /* get pointer on pbuf from srv structure */
    ptr = srv->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, TCP_WRITE_FLAG_COPY);
    
    if (wr_err == ERR_OK) {
      u16_t plen;
      u8_t freed;

      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      srv->p = ptr->next;
      
      if(srv->p != NULL) {
        /* increment reference count for srv->p */
        pbuf_ref(srv->p);
      }
      
      /* chop first pbuf from chain */
      do {
        /* try hard to free pbuf */
        freed = pbuf_free(ptr);
      } while(freed == 0);

       /* we can read more data now */
       tcp_recved(tpcb, plen);
     } else if(wr_err == ERR_MEM) {
        /* we are low on memory, try later / harder, defer to poll */
       srv->p = ptr;
     }
  }
}

/**
  * @brief  This functions closes the tcp connection
  * @param  tcp_pcb: pointer on the tcp connection
  * @param  srv: pointer on server_handler structure
  * @retval None
  */
static void tcp_server_connection_close(struct tcp_pcb *tpcb, struct tcp_server_struct *srv) {
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete srv structure */
  if (srv != NULL) {
    mem_free(srv);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);
}

#endif /* LWIP_TCP */
