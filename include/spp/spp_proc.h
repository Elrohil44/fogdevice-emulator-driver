#ifndef __SPP_PROC_H__
#define __SPP_PROC_H__

void sppTxMultichannel(uint8_t dst, uint8_t src, uint16_t mask,
	uint8_t cmd, uint8_t* params, uint8_t paramLen);
	
void sppAllOff(uint8_t dst, uint8_t src);

void sppWriteDelays(uint8_t* params);

int sppReadRegs(uint8_t set,uint8_t* params);

void sppAllOn_Alt(void);
void sppAllOff_Alt(void);

void sppToggle(uint8_t ch);
void sppToggle_Alt(uint8_t ch);

void sppWriteCmdMulti(uint8_t cmd, uint8_t* params, uint8_t pnb);

#endif

