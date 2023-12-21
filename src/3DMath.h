#ifndef _3DMATH_H
#define _3DMATH_H
#include <utility>
#include <memory.h>
#include <string.h>
#include <Types.h>
#include "GBI.h"

#include "rtm/scalarf.h"
#include "rtm/matrix4x4f.h"

void MultMatrix(float m0[4][4], float m1[4][4], float dest[4][4]);
void MultMatrix2(float m0[4][4], float m1[4][4]);
void TransformVectorNormalize(float vec[3], float mtx[4][4]);
void InverseTransformVectorNormalize(float src[3], float dst[3], float mtx[4][4]);
void InverseTransformVectorNormalizeN(float src[][3], float dst[][3], float mtx[4][4], u32 count);
void Normalize(float v[3]);
float DotProduct(const float v0[3], const float v1[3]);
void CopyMatrix( float m0[4][4], float m1[4][4]);

static inline rtm::matrix4x4f toRtmMatrix(const float m[4][4])
{
	return rtm::matrix_set(rtm::vector_load(m[0])
						 , rtm::vector_load(m[1])
					     , rtm::vector_load(m[2])
					     , rtm::vector_load(m[3]));
}

static inline void toFloatMatrix(rtm::matrix4x4f m, float dest[4][4])
{
	rtm::vector_store(m.x_axis, dest[0]);
	rtm::vector_store(m.y_axis, dest[1]);
	rtm::vector_store(m.z_axis, dest[2]);
	rtm::vector_store(m.w_axis, dest[3]);
}

inline float DotProduct(const float v0[3], const float v1[3])
{
	auto rv0 = rtm::vector_load3(v0);
	auto rv1 = rtm::vector_load3(v1);
	return rtm::vector_dot3(rv0, rv1);
}

inline float GetFloatMatrixElement(s16 _int, u16 _fract)
{
	const s32 element = (_int << 16) | _fract;
	return _FIXED2FLOAT(element, 16);
}

inline std::pair<s16, u16> GetIntMatrixElement(f32 _elem)
{
	const s32 value = static_cast<s32>(_elem * 65536.0f);
	return std::pair<s16, u16>(static_cast<s16>(value >> 16), static_cast<u16>(value & 0xFFFF));
}

static void floatVerify(const float* a, const float* b, int cnt)
{
	for (int i = 0; i < cnt; i++)
	{
		f32 diff = fabs(a[i] - b[i]);
		if (diff > 0.0001f)
		{
			__debugbreak();
		}
	}
}

#endif
