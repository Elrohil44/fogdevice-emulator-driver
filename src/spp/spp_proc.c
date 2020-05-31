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




#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include "spp/spp.h"

T_sppPacket packet;

extern const uint8_t DUT_ADDR;

#define SRC_ADDR		SPP_ADDRESS

const uint8_t CHAN_NB = 12;


void sppWriteCmdMulti(uint8_t cmd, uint8_t* params, uint8_t pnb)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = pnb+1;
	packet.payload[0] = cmd;
	
	for(int i=0;i<pnb;i++)
	{
		packet.payload[1+i] = params[i];
	}
	
	sppTx(&packet);
}

void sppToggle(uint8_t ch)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_CHAN_TOGGLE | (ch&0xF);
	sppTx(&packet);
}

void sppToggle_Alt(uint8_t ch)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_CHAN_TOGGLE_ALT | (ch&0xF);
	sppTx(&packet);
}


void sppAllOn_Alt(void)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_ALL_ON_ALT;
	sppTx(&packet);
}

void sppAllOn(void)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_ALL_ON;
	sppTx(&packet);
}


void sppAllOff_Alt(void)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_ALL_OFF_ALT;
	sppTx(&packet);
}


int sppReadRegs(uint8_t set,uint8_t* params)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 1;
	packet.payload[0] = SPP_ID_REQ | (set&0x0F);
	sppTx(&packet);
	
	return 0;	
}



void sppTxMultichannel(uint8_t dst, uint8_t src, uint16_t mask,
	uint8_t cmd, uint8_t* params, uint8_t paramLen)
{
	packet.dstAddr = dst;
	packet.srcAddr = src;
	packet.len = 4+paramLen;
	packet.payload[0] = SPP_ID_MULTICHANNEL;
	packet.payload[1] = (mask >> 8) & 0xFF;	//maskH
	packet.payload[2] = mask & 0xFF;
	packet.payload[3] = cmd;
	for(int i=0;i<paramLen;i++)
	{
		packet.payload[4+i] = params[i];	
	}
	sppTx(&packet);
}

void sppAllOff(uint8_t dst, uint8_t src)
{
	packet.dstAddr = dst;
	packet.srcAddr = src;
	packet.len = 1;
	packet.payload[0] = SPP_ID_ALL_OFF;
	sppTx(&packet);
}

void sppWriteDelays(uint8_t* params)
{
	packet.dstAddr = DUT_ADDR;
	packet.srcAddr = SRC_ADDR;
	packet.len = 9;
	packet.payload[0] = SPP_ID_TRANSITION;
	
	for(int i=0;i<CHAN_NB;i++)
	{
		packet.payload[1+i] = params[i];
	}
	
	sppTx(&packet);
}
