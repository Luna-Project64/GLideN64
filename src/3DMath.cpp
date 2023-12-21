#include <math.h>
#include "3DMath.h"

#include "rtm/matrix3x3f.h"

#ifdef WIN32_ASM
#undef WIN32_ASM
#endif

void TransformVectorNormalize(float vec[3], rtm::matrix4x4f mtx)
{
	auto rvec = rtm::vector_load3(vec);
	rtm::matrix3x3f mtx3 = rtm::matrix_cast(mtx);

	auto rvres = rtm::matrix_mul_vector3(rvec, mtx3);
	auto rvresn = rtm::vector_normalize3(rvres, rvres);
	rtm::vector_store3(rvresn, vec);
}

void InverseTransformVectorNormalize(float src[3], float dst[3], rtm::matrix4x4f mtx)
{
	auto rsrc = rtm::vector_load3(src);
	rtm::matrix3x3f mtx3 = rtm::matrix_cast(mtx);
	auto mtx3t = rtm::matrix_transpose(mtx3);

	auto rvres = rtm::matrix_mul_vector3(rsrc, mtx3t);
	auto rvresn = rtm::vector_normalize3(rvres, rvres);
	rtm::vector_store3(rvresn, dst);
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

	auto rv = rtm::vector_load3(v);
	rv = rtm::vector_normalize3(rv, rv);
	rtm::vector_store3(rv, v);

	floatVerify(v, vv, sizeof(vv) / sizeof(*vv));
}

void InverseTransformVectorNormalizeN(float src[][3], float dst[][3], rtm::matrix4x4f mtx, u32 count)
{
	for (u32 i = 0; i < count; i++)
	{
		InverseTransformVectorNormalize((float(*))src[i], (float(*))dst[i], mtx);
	}
}
