#include <math.h>
#include "3DMath.h"

#include "rtm/matrix3x3f.h"

#ifdef WIN32_ASM
#undef WIN32_ASM
#endif

void MultMatrix(float m0[4][4], float m1[4][4], float dest[4][4])
{
	float vdest[4][4];
	int i;
	for (i = 0; i < 4; i++)
	{
		vdest[0][i] = m0[0][i] * m1[0][0] + m0[1][i] * m1[0][1] + m0[2][i] * m1[0][2] + m0[3][i] * m1[0][3];
		vdest[1][i] = m0[0][i] * m1[1][0] + m0[1][i] * m1[1][1] + m0[2][i] * m1[1][2] + m0[3][i] * m1[1][3];
		vdest[2][i] = m0[0][i] * m1[2][0] + m0[1][i] * m1[2][1] + m0[2][i] * m1[2][2] + m0[3][i] * m1[2][3];
		vdest[3][i] = m0[3][i] * m1[3][3] + m0[2][i] * m1[3][2] + m0[1][i] * m1[3][1] + m0[0][i] * m1[3][0];
	}

	toFloat44(rtm::matrix_mul(tortm44(m1), tortm44(m0)), dest);
	floatVerify((float*)dest, (float*)vdest, sizeof(vdest) / sizeof(*vdest));
}

void MultMatrix2(float m0[4][4], float m1[4][4])
{
	toFloat44(rtm::matrix_mul(tortm44(m1), tortm44(m0)), m0);
}

void TransformVectorNormalize(float vec[3], float mtx[4][4])
{
	float vres[3];
	vres[0] = mtx[0][0] * vec[0] + mtx[1][0] * vec[1] + mtx[2][0] * vec[2];
	vres[1] = mtx[0][1] * vec[0] + mtx[1][1] * vec[1] + mtx[2][1] * vec[2];
	vres[2] = mtx[0][2] * vec[0] + mtx[1][2] * vec[1] + mtx[2][2] * vec[2];

	float vresn[3] = { vres[0], vres[1], vres[2] };
	Normalize(vresn);

	auto rvec = tortm3(vec);
	auto rmtx = tortm44(mtx);
	rtm::matrix3x3f rmtx3 = rtm::matrix_cast(rmtx);

	auto rvres = rtm::matrix_mul_vector3(rvec, rmtx3);
	auto rvresn = rtm::vector_normalize3(rvres, rvres);
	toFloat3(rvresn, vec);

	floatVerify(vec, vresn, sizeof(vres) / sizeof(*vres));
}

void InverseTransformVectorNormalize(float src[3], float dst[3], float mtx[4][4])
{
	float vdst[3];
	vdst[0] = mtx[0][0] * src[0] + mtx[0][1] * src[1] + mtx[0][2] * src[2];
	vdst[1] = mtx[1][0] * src[0] + mtx[1][1] * src[1] + mtx[1][2] * src[2];
	vdst[2] = mtx[2][0] * src[0] + mtx[2][1] * src[1] + mtx[2][2] * src[2];
	Normalize(vdst);

	auto rsrc = tortm3(src);
	auto rmtx = tortm44(mtx);
	rtm::matrix3x3f rmtx3 = rtm::matrix_cast(rmtx);
	auto rmtx3t = rtm::matrix_transpose(rmtx3);

	auto rvres = rtm::matrix_mul_vector3(rsrc, rmtx3t);
	auto rvresn = rtm::vector_normalize3(rvres, rvres);
	toFloat3(rvresn, dst);

	floatVerify(vdst, dst, sizeof(vdst) / sizeof(*vdst));
}

void Normalize(float v[3])
{
	float vv[3] = { v[0], v[1], v[2] };
	float len;
	len = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (len != 0.0) {
		len = sqrtf(len);
		vv[0] /= len;
		vv[1] /= len;
		vv[2] /= len;
	}

	auto rv = tortm3(v);
	rv = rtm::vector_normalize3(rv, rv);
	toFloat3(rv, v);

	floatVerify(v, vv, sizeof(vv) / sizeof(*vv));
}

void InverseTransformVectorNormalizeN(float src[][3], float dst[][3], float mtx[4][4], u32 count)
{
	for (u32 i = 0; i < count; i++)
	{
		InverseTransformVectorNormalize((float(*))src[i], (float(*))dst[i], mtx);
	}
}

void CopyMatrix( float m0[4][4], float m1[4][4] )
{
#ifdef WIN32_ASM
	__asm {
		mov		esi, [m1]
		mov		edi, [m0]

		mov		eax, dword ptr [esi+00h]
		mov		dword ptr [edi+00h], eax
		mov		eax, dword ptr [esi+04h]
		mov		dword ptr [edi+04h], eax
		mov		eax, dword ptr [esi+08h]
		mov		dword ptr [edi+08h], eax
		mov		eax, dword ptr [esi+0Ch]
		mov		dword ptr [edi+0Ch], eax

		mov		eax, dword ptr [esi+10h]
		mov		dword ptr [edi+10h], eax
		mov		eax, dword ptr [esi+14h]
		mov		dword ptr [edi+14h], eax
		mov		eax, dword ptr [esi+18h]
		mov		dword ptr [edi+18h], eax
		mov		eax, dword ptr [esi+1Ch]
		mov		dword ptr [edi+1Ch], eax

		mov		eax, dword ptr [esi+20h]
		mov		dword ptr [edi+20h], eax
		mov		eax, dword ptr [esi+24h]
		mov		dword ptr [edi+24h], eax
		mov		eax, dword ptr [esi+28h]
		mov		dword ptr [edi+28h], eax
		mov		eax, dword ptr [esi+2Ch]
		mov		dword ptr [edi+2Ch], eax

		mov		eax, dword ptr [esi+30h]
		mov		dword ptr [edi+30h], eax
		mov		eax, dword ptr [esi+34h]
		mov		dword ptr [edi+34h], eax
		mov		eax, dword ptr [esi+38h]
		mov		dword ptr [edi+38h], eax
		mov		eax, dword ptr [esi+3Ch]
		mov		dword ptr [edi+3Ch], eax
	}
#else
	memcpy( m0, m1, 16 * sizeof( float ) );
#endif // WIN32_ASM
}
