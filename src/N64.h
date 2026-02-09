#ifndef N64_H
#define N64_H

#include "Types.h"
#include <stdint.h>

#define MI_INTR_DP		0x20		// Bit 5: DP intr

struct N64Regs
{
	u32 *MI_INTR;

	u32 *DPC_START;
	u32 *DPC_END;
	u32 *DPC_CURRENT;
	u32 *DPC_STATUS;
	u32 *DPC_CLOCK;
	u32 *DPC_BUFBUSY;
	u32 *DPC_PIPEBUSY;
	u32 *DPC_TMEM;

	u32 *_VI_STATUS;
	u32 *_VI_ORIGIN;
	u32 *_VI_WIDTH;
	u32 *_VI_INTR;
	u32 *_VI_V_CURRENT_LINE;
	u32 *_VI_TIMING;
	u32 *_VI_V_SYNC;
	u32 *_VI_H_SYNC;
	u32 *_VI_LEAP;
	u32 *_VI_H_START;
	u32 *_VI_V_START;
	u32 *_VI_V_BURST;
	u32 *_VI_X_SCALE;
	u32 *_VI_Y_SCALE;

	u32 *SP_STATUS;
};

struct VIRegsSample
{
	u32 VI_STATUS;
	u32 VI_ORIGIN;
	u32 VI_WIDTH;
	u32 VI_V_CURRENT_LINE;
	u32 VI_TIMING;
	u32 VI_V_SYNC;
	u32 VI_H_SYNC;
	u32 VI_LEAP;
	u32 VI_H_START;
	u32 VI_V_START;
	u32 VI_V_BURST;
	u32 VI_X_SCALE;
	u32 VI_Y_SCALE;
};

extern N64Regs REG;
extern u8 *HEADER;
extern u8 *DMEM;
extern u8 *IMEM;
extern u8 *RDRAM;
extern u64 TMEM[512];
extern u32 RDRAMSize;
extern bool ConfigOpen;

extern "C"
{
	extern uint32_t LegacySm64ToolsHacks;
	extern uint32_t DepthFragmentWrite;
}

static inline VIRegsSample sampleVI()
{
	VIRegsSample sample;
	sample.VI_STATUS = *(REG._VI_STATUS);
	sample.VI_ORIGIN = *(REG._VI_ORIGIN);
	sample.VI_WIDTH = *(REG._VI_WIDTH);
	sample.VI_V_CURRENT_LINE = *(REG._VI_V_CURRENT_LINE);
	sample.VI_TIMING = *(REG._VI_TIMING);
	sample.VI_V_SYNC = *(REG._VI_V_SYNC);
	sample.VI_H_SYNC = *(REG._VI_H_SYNC);
	sample.VI_LEAP = *(REG._VI_LEAP);
	sample.VI_H_START = *(REG._VI_H_START);
	sample.VI_V_START = *(REG._VI_V_START);
	sample.VI_V_BURST = *(REG._VI_V_BURST);
	sample.VI_X_SCALE = *(REG._VI_X_SCALE);
	sample.VI_Y_SCALE = *(REG._VI_Y_SCALE);

	if (LegacySm64ToolsHacks)
	{
		if (sample.VI_V_SYNC == 0x0627) sample.VI_V_SYNC = 0x0834;
	}

	return sample;
}

struct TMEMCacheHashEntry
{
	u32 off;
	u32 size;
	u32 hash;
};
extern TMEMCacheHashEntry TMEMCacheHash;

static inline void tmemCacheHashInvalidate()
{
	TMEMCacheHash.off = -1;
}

static inline void tmemCacheHashSet(u32 off, u32 size, u32 hash)
{
	TMEMCacheHash.off = off;
	TMEMCacheHash.size = size;
	TMEMCacheHash.hash = hash;
}

static inline const u32* tmemCacheHashTryGet(u32 off, u32 size)
{
	if (TMEMCacheHash.off == off && TMEMCacheHash.size == size)
		return &TMEMCacheHash.hash;
	else
		return nullptr;
}

#endif

