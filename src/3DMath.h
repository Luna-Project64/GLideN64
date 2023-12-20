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

static inline rtm::vector4f tortm4(const float v[4])
{
	return rtm::vector_set(v[0], v[1], v[2], v[3]);
}

static inline rtm::vector4f tortm3(const float v[3])
{
	return rtm::vector_set(v[0], v[1], v[2]);
}

static inline rtm::matrix4x4f tortm44(const float m[4][4])
{
	return rtm::matrix_set(tortm4(m[0]), tortm4(m[1]), tortm4(m[2]), tortm4(m[3]));
}

static inline void toFloat44(rtm::matrix4x4f m, float dest[4][4])
{
	auto x = m.x_axis;
	dest[0][0] = rtm::vector_get_x(x);
	dest[0][1] = rtm::vector_get_y(x);
	dest[0][2] = rtm::vector_get_z(x);
	dest[0][3] = rtm::vector_get_w(x);
	auto y = m.y_axis;
	dest[1][0] = rtm::vector_get_x(y);
	dest[1][1] = rtm::vector_get_y(y);
	dest[1][2] = rtm::vector_get_z(y);
	dest[1][3] = rtm::vector_get_w(y);
	auto z = m.z_axis;
	dest[2][0] = rtm::vector_get_x(z);
	dest[2][1] = rtm::vector_get_y(z);
	dest[2][2] = rtm::vector_get_z(z);
	dest[2][3] = rtm::vector_get_w(z);
	auto w = m.w_axis;
	dest[3][0] = rtm::vector_get_x(w);
	dest[3][1] = rtm::vector_get_y(w);
	dest[3][2] = rtm::vector_get_z(w);
	dest[3][3] = rtm::vector_get_w(w);
}

static inline void toFloat3(rtm::vector4f v, float dest[3])
{
	dest[0] = rtm::vector_get_x(v);
	dest[1] = rtm::vector_get_y(v);
	dest[2] = rtm::vector_get_z(v);
}

static inline void toFloat4(rtm::vector4f v, float dest[4])
{
	dest[0] = rtm::vector_get_x(v);
	dest[1] = rtm::vector_get_y(v);
	dest[2] = rtm::vector_get_z(v);
	dest[3] = rtm::vector_get_w(v);
}

inline float DotProduct(const float v0[3], const float v1[3])
{
	auto rv0 = tortm3(v0);
	auto rv1 = tortm3(v1);
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
