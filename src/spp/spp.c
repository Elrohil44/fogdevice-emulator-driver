/*
* The MIT License (MIT)
* Copyright (c) 2019 Robert Brzoza-Woch
* Permission is hereby granted, free of charge, to any person obtaining 
* a copy of this software and associated documentation files (the "Software"), 
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
* OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * =====================================================================
 * ====      Simple Packet Protocol version 2.1 (Linux version)     ====
 * =====================================================================
 * */


#include "spp/spp.h"
#include "rs232.h"
#include <stdio.h>


#define SPP_START_DELIMITER         0x00
#define SPP_START_DISPATCH          0x80
#define SPP_REPLACE_START_DEL       0x81
#define SPP_REPLACE_START_DISPATCH  0x82


enum{
  SPP_STATE_IDLE = 0,
  SPP_STATE_DSTADDR = 1,
  SPP_STATE_SRCADDR = 2,
  SPP_STATE_LEN = 3,
  SPP_STATE_CMDID = 4,
  SPP_STATE_PAYLOAD = 5,
  SPP_STATE_CRC = 6
};

static uint8_t sppRxState = SPP_STATE_IDLE;

extern int port_id;

static T_sppPacket rxPacket;  //rx buffer

static void txb(uint8_t b);
static void rxFsm(uint8_t rx);
void sppRxedPacket_callback(T_sppPacket* pPacket) __attribute__((weak));

static uint8_t crcByte(uint8_t crc, uint8_t data);

static volatile uint8_t rxReady = 0;


void sppRxedPacket_callback(T_sppPacket* pPacket)
{
  printf("sppRxedPacket_callback - weak\n");
}


void sppInit(void)
{
  sppRxState = SPP_STATE_IDLE;
}

int sppProcess(void)
{
  unsigned char buf[256];
  
  int len = RS232_PollComport(port_id,buf,256);
  
  if(len)
  {
    for(int i=0;i<len;i++)
    {
      printf("%02X ",buf[i]);
      rxFsm(buf[i]);
    }
    return 1;
  }
  
  return 0;
}


uint8_t sppIsIdle(void)
{
  if( sppRxState == SPP_STATE_IDLE ) return 1; else return 0;
}


#ifndef SPP_IDLE_TIMEOUT
  #define SPP_IDLE_TIMEOUT  100
  #warning SPP_IDLE_TIMEOUT not defined. Default value will be used.
#endif

void sppIdleTimeout(void)
{
  static uint16_t idleCntr;
  if(sppIsIdle())
  {
    idleCntr=0;
  }
  else
  {
    if(idleCntr < SPP_IDLE_TIMEOUT)
    {
      idleCntr++;
    }
    else
    {
      idleCntr=0;
      sppRxState = SPP_STATE_IDLE;
      SPP_LED_BUSY_OFF;
    }
  }
}


//tx with dispatch
static void txwd(uint8_t b)
{
  switch(b)
  {
    case SPP_START_DELIMITER:
      txb(SPP_START_DISPATCH);
      txb(SPP_REPLACE_START_DEL);
      return;
    
    case SPP_START_DISPATCH:
      txb(SPP_START_DISPATCH);
      txb(SPP_REPLACE_START_DISPATCH);
      return;
    
    default:
      txb(b);
      return;
  }
}

//tx acknowledge
//input: packet to be acked
//returns: 0 on error, nonzero if OK
uint8_t sppTxAck(T_sppPacket* pPacket)
{
  if((pPacket->dstAddr & 0xF0)==0xF0) return 0; //don't ack broadcasts
  if((pPacket->dstAddr & 0x0F)==0x0F) return 0; //don't ack broadcasts
  pPacket->dstAddr = pPacket->srcAddr;      //swap addresses = return to sender
  pPacket->len = 0;
  pPacket->cmdid = SPP_ID_ACK;
  return(sppTx(pPacket));
}



//the receiver is in idle or ready state: 1=yes, 0=no
static uint8_t busIsFree(void)
{
  if(sppRxState == SPP_STATE_IDLE) return 1; else return 0;
}



//wait up to c.a. 100 ms for the bus to be idle
//returns 1 if the bus has been released before the timeout expired
//returns 0 if the timeout has expired and the bus is still busy
static uint8_t waitForFreeBus(void) __attribute__((unused));

static uint8_t waitForFreeBus(void)
{
  uint16_t timeout = 100;
  while( timeout-- )
  {
    if(busIsFree())
    {
      return 1;
    }
    delay_ms(1);
  }
  return 0;
}


//returns zero if address is matched
static uint8_t checkDstAddress(uint8_t addr)
{
  //broadcast to all instancess of a class (addr[3:0]==0xF and addr[7:4] matches SPP_ADDRESS[7:4])
  if( ((addr&0x0F)==0x0F) && ((addr&0xF0)==(SPP_ADDRESS&0xF0)) ) return 0;
  
  //broadcast to all devices in the network
  if(addr==0xFF) return 0;
  
  //1:1 match
  if( addr==SPP_ADDRESS ) return 0;
  
  //mismatch
  return 1;
}


static uint8_t crcByte(uint8_t crc, uint8_t data)
{
  uint8_t i;
  crc = crc ^ data;
  for(i=0;i<8;i++)
  {
    if(crc & 0x01)
      crc = (crc >> 1) ^ 0x8C;
    else
      crc >>= 1;
  }
  return(crc);
}


//tx byte
static void txb(uint8_t b)
{
  RS232_SendByte(port_id,b);
}


//input: packet to be sent
//returns: 0 on error, nonzero if OK
uint8_t sppTx(T_sppPacket* pPacket)
{
  SPP_LED_BUSY_ON;
  
  //do not perform bus-free checking for Linux implementation
  #if 0
  uint8_t waitRetriesLeft = 3;
  while((busIsFree()==0) && (waitRetriesLeft--))
  {
    waitForFreeBus();
    
    //other devices may attempt to tx just after the bus has become free
    //so we wait the unique period of time (directly dependent the on device address)
    //then we check the bus again
    delay_ms( SPP_ADDRESS * 2 );
  }
  
  //at this point we ignore the busy condition ->
  //...we assume that there was an error on the bus
  //(an unfinished packet transmission could cause that)
  #endif
  
  
  //take over the bus
  txb(SPP_START_DELIMITER);
  
  uint8_t crc = 0;
  
  txwd(pPacket->dstAddr);
  crc = crcByte(crc,pPacket->dstAddr);
  
  txwd(SPP_ADDRESS);
  crc = crcByte(crc,SPP_ADDRESS);
  
  txwd(~(pPacket->len));
  crc = crcByte(crc,pPacket->len);
  
  txwd(~pPacket->cmdid);
  crc = crcByte(crc,pPacket->cmdid);
  
  for(uint8_t i=0;i<pPacket->len;i++)
  {
    txwd(~pPacket->payload[i]);
    crc = crcByte(crc,pPacket->payload[i]);
  }
  
  txwd(crc);
  
  SPP_LED_BUSY_OFF;
  
  return 0;
}

//rx packet
uint8_t sppRx(T_sppPacket* pPacket)
{
  unsigned char buf[256];
  
  int len = RS232_PollComport(port_id,buf,256);
  
  if(len)
  {
    for(int i=0;i<len;i++)
    {
      printf("%02X ",buf[i]);
      rxFsm(buf[i]);
    }
    return 1;
  }
  
  return 0;
}




static void rxFsm(uint8_t rx)
{
  static uint8_t dispatch;
  static uint8_t crc;
  static uint8_t payloadIdx;
  
  //address mismatch flag - track further packet bytes but ignore their content
  static uint8_t mismatch;
  
  switch(rx)
  {
    case SPP_START_DELIMITER:  //rxed start byte
      sppRxState = SPP_STATE_DSTADDR;
      SPP_LED_BUSY_ON;
      dispatch = 0;
      mismatch = 0;
      return; //exit!
    
    case SPP_START_DISPATCH:
      dispatch = 1;
      return; //exit!
    
    case SPP_REPLACE_START_DEL:
      if(dispatch) rx = SPP_START_DELIMITER;
      break;
      
    case SPP_REPLACE_START_DISPATCH:
      if(dispatch) rx = SPP_START_DISPATCH;
      break;
  }
  
  dispatch = 0;
  
  switch(sppRxState)
  {
    case SPP_STATE_DSTADDR:
      #if SPP_SNIFF_MODE
        mismatch = 0;
      #else
        mismatch = checkDstAddress(rx);
      #endif
      rxPacket.dstAddr=rx;
      crc = crcByte(0,rx);  //start computing crc
      sppRxState = SPP_STATE_SRCADDR;
      
      break;
    
    case SPP_STATE_SRCADDR:
      rxPacket.srcAddr=rx;
      crc = crcByte(crc,rx);
      sppRxState = SPP_STATE_LEN;
      break;
    
    case SPP_STATE_LEN:
      rx = ~rx;
      if(rx > (SPP_PAYLOAD_LEN-1))
      {
        mismatch = 1;
      }
      rxPacket.len = rx;
      sppRxState = SPP_STATE_CMDID;
      crc = crcByte(crc,rx);
      break;

    case SPP_STATE_CMDID:
      rx = ~rx;
      rxPacket.cmdid = rx;
      payloadIdx = 0;
      sppRxState = SPP_STATE_PAYLOAD;
      crc = crcByte(crc,rx);
      break;

    
    case SPP_STATE_PAYLOAD:
      if(payloadIdx>=rxPacket.len)  //end of payload?
      {
        #if !SPP_IGNORE_CRC
        if(crc != rx)
        {
          //Discard packet and go to IDLE state
          sppRxState = SPP_STATE_IDLE;
          SPP_LED_BUSY_OFF;
          return;
        }
        #endif
        
        if(mismatch==0)
        {
          //set rx state to "ready"
          sppRxState = SPP_STATE_IDLE;
          rxReady = 1;
          
          sppRxedPacket_callback(&rxPacket);
          
        }
        
        sppRxState = SPP_STATE_IDLE;
        SPP_LED_BUSY_OFF;
        
        return;
        
      }
      
      //is the packet for me?
      if(mismatch==0)
      {
        rx = ~rx;
        //write rxed bytes to the buffer and advance the pointer
        if(payloadIdx < (SPP_PAYLOAD_LEN-1))
          rxPacket.payload[payloadIdx++] = rx;
      }
      else
      {
        //keep track of a number of bytes rxed
        payloadIdx++;
      }
      
      crc = crcByte(crc,rx);
      break;
  }
  

}

