#include "RSP.h"
#include "3DMath.h"

rtm::matrix4x4f RSP_LoadMatrix( u32 address )
{
    struct _N64Matrix
    {
        s16 integer[4][4];
        u16 fraction[4][4];
    } *n64Mat = (struct _N64Matrix *)&RDRAM[address];

    float mtx[4][4];
    for (u32 i = 0; i < 4; i++)
        for (u32 j = 0; j < 4; j++)
			mtx[i][j] = GetFloatMatrixElement(n64Mat->integer[i][j ^ 1], n64Mat->fraction[i][j ^ 1]);

#if 1
    // this is hackery but rtm does not allow short convs so i am doing some SSE moonspeak for a bit
    __m128i _integer_s16[2];
    _integer_s16[0] = _mm_loadu_si128((__m128i*)n64Mat->integer[0]);
    _integer_s16[1] = _mm_loadu_si128((__m128i*)n64Mat->integer[2]);

    __m128i _fraction_u16[2];
    _fraction_u16[0] = _mm_loadu_si128((__m128i*)n64Mat->fraction[0]);
    _fraction_u16[1] = _mm_loadu_si128((__m128i*)n64Mat->fraction[2]);

    __m128i _unpacked_s32[4];
    _unpacked_s32[0] = _mm_unpacklo_epi16(_fraction_u16[0], _integer_s16[0]);
    _unpacked_s32[1] = _mm_unpackhi_epi16(_fraction_u16[0], _integer_s16[0]);
    _unpacked_s32[2] = _mm_unpacklo_epi16(_fraction_u16[1], _integer_s16[1]);
    _unpacked_s32[3] = _mm_unpackhi_epi16(_fraction_u16[1], _integer_s16[1]);

    rtm::vector4f axis[4];
    axis[0] = _mm_cvtepi32_ps(_unpacked_s32[0]);
    axis[1] = _mm_cvtepi32_ps(_unpacked_s32[1]);
    axis[2] = _mm_cvtepi32_ps(_unpacked_s32[2]);
    axis[3] = _mm_cvtepi32_ps(_unpacked_s32[3]);

    rtm::scalarf recip = rtm::scalar_set(FIXED2FLOATRECIP16);

    rtm::vector4f saxis[4];
    saxis[0] = rtm::vector_mul(axis[0], recip);
    saxis[1] = rtm::vector_mul(axis[1], recip);
    saxis[2] = rtm::vector_mul(axis[2], recip);
    saxis[3] = rtm::vector_mul(axis[3], recip);

    rtm::vector4f maxis[4];
    maxis[0] = rtm::vector_mix<rtm::mix4::b, rtm::mix4::a, rtm::mix4::d, rtm::mix4::c>(saxis[0], saxis[0]);
    maxis[1] = rtm::vector_mix<rtm::mix4::b, rtm::mix4::a, rtm::mix4::d, rtm::mix4::c>(saxis[1], saxis[1]);
    maxis[2] = rtm::vector_mix<rtm::mix4::b, rtm::mix4::a, rtm::mix4::d, rtm::mix4::c>(saxis[2], saxis[2]);
    maxis[3] = rtm::vector_mix<rtm::mix4::b, rtm::mix4::a, rtm::mix4::d, rtm::mix4::c>(saxis[3], saxis[3]);

    rtm::matrix4x4f rmtx = rtm::matrix_set(maxis[0], maxis[1], maxis[2], maxis[3]);

    float vmtx[4][4];
    toFloatMatrix(rmtx, vmtx);
    floatVerify((const float*) mtx, (const float*) vmtx, sizeof(mtx) / sizeof(float));

    return rmtx;
#else
    return toRtmMatrix(mtx);
#endif
}
