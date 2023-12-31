#include "RSP.h"
#include "3DMath.h"

Mtx RSP_LoadMatrix( u32 address )
{
    struct _N64Matrix
    {
        s16 integer[4][4];
        u16 fraction[4][4];
    } *n64Mat = (struct _N64Matrix *)&RDRAM[address];

    Mtx mtx;
    for (u32 i = 0; i < 4; i++)
        for (u32 j = 0; j < 4; j++)
			mtx.v[i][j] = GetFloatMatrixElement(n64Mat->integer[i][j ^ 1], n64Mat->fraction[i][j ^ 1]);

    return mtx;
}
