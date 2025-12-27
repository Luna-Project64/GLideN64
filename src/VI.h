#ifndef VI_H
#define VI_H
#include "Types.h"

struct VIInfo
{
	u32 width, widthPrev, height, real_height;
	f32 rwidth, rheight;
	u32 lastOrigin;
	bool interlaced;
	bool PAL;

	VIInfo() :
		width(0), widthPrev(0), height(0), real_height(0), rwidth(0), rheight(0),
		lastOrigin(-1), interlaced(false), PAL(false)
	{}
};

extern VIInfo VI;

struct VIRegsSample;
void VI_UpdateSize(const VIRegsSample& regs);
void VI_UpdateScreen(const VIRegsSample& regs);
u16 VI_GetMaxBufferHeight(u16 _width);

#endif

