/*
* The MIT License (MIT)
* Copyright (c) 2016-2019 Robert Brzoza-Woch
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



//sample cmdid field contents

//global control IDs
#define SPP_ID_ACK				0x00	//packet is ACK
#define SPP_ID_EXT				0x01	//extended command in payload
#define SPP_ID_NITEMODE_ON		0x02	//enter night mode
#define SPP_ID_NITEMODE_OFF		0x03	//exit night mode
#define SPP_ID_ALL_ON			0x04	//all channels ON
#define SPP_ID_ALL_OFF			0x05	//all channels OFF
#define SPP_ID_ALL_ON_ALT		0x06	//all channels ON (alternative)
#define SPP_ID_ALL_OFF_ALT		0x07	//all channels OFF (alternative)
#define SPP_ID_MULTICHANNEL		0x08	//selective ON/OFF/Toggle: determined by payload[2:0]
#define SPP_ID_TEXT				0x0A	//payload[0]-id, payload[1,...]-text
#define SPP_ID_MULTICAST		0x0F	//reserved for multicast messages

//channel command IDs
#define SPP_ID_CHAN_ON			0x10
#define SPP_ID_CHAN_OFF			0x20
#define SPP_ID_CHAN_TOGGLE		0x30

#define SPP_ID_CHAN_ON_ALT		0x40
#define SPP_ID_CHAN_OFF_ALT		0x50
#define SPP_ID_CHAN_TOGGLE_ALT	0x60

#define SPP_ID_CHAN_UP			0x70	//increase value for a channel
#define SPP_ID_CHAN_DN			0x80	//decrease value for a channel
#define SPP_ID_CHAN_VALUE		0x90	//set value for a channel, value in payload[0]
#define SPP_ID_CHAN_VALUE_ALT	0xA0	//set value for a channel, value in payload[0]

#define SPP_ID_TRANSITION		0xB0	//set default transition speed for each channel
#define SPP_ID_POWER_DEFAULT	0xC0	//set power level applied with "on" and "toggle" commands

//Data IDs
#define SPP_ID_TIME				0xD0	//time info
#define SPP_ID_CHANNEL_STATE	0xD1	//state info for channel devices
#define SPP_ID_TEMPERATURE		0xD2	//temperature
#define SPP_ID_PRESSURE			0xD3	//pressure
#define SPP_ID_PUMP_STATE		0xD4	//water pump
#define SPP_ID_HUMIDITY			0xD5	//humidity
#define SPP_ID_ALARM_STATE		0xDA	//state info for alarm

//request-response
#define SPP_ID_REQ				0xE0
#define SPP_ID_RESP				0xF0
