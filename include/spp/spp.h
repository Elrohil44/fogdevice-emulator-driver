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
 * ====            Simple Packet Protocol version 2                 ====
 * =====================================================================
 * */


#ifndef __SPP_H__
#define __SPP_H__

#include <inttypes.h>
#include <string.h>

#include "spp_cfg.h"

#include "spp_cmdid.h"

#define SPP_BCAST_IN_CLASS			0x0F
#define SPP_BCAST_ALL				0xFF

#define SPP_RESPONSE_DELAY_MS		(150 + SPP_ADDRESS)
#define SPP_TX_DELAY_MS				2		//delay between consecutive transmissions

typedef struct __attribute__((packed))
{
	uint8_t dstAddr;
	uint8_t srcAddr;
	uint8_t len;
	uint8_t cmdid;
	uint8_t payload[SPP_PAYLOAD_LEN];
}T_sppPacket;

#define SPP_HEADER_SIZE		3	//packet header size: dstAddr + scrAddc + len

void sppInit(void);
uint8_t sppRx(T_sppPacket* pPacket);
int sppProcess(void);
uint8_t sppTx(T_sppPacket* pPacket);
uint8_t sppTxAck(T_sppPacket* pPacket);
uint8_t sppIsIdle(void);
void sppIdleTimeout(void);


#endif

