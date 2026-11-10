#include "extdll.h"
#include "util.h"
#include "VectorRotation.h"

#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
#include <xmmintrin.h>
#endif

static const double ROTATION_HALF_TURN = 180.0;

// Preserve the SSE operand order when both operands contain NaNs
float RotationAdd(float first, float second)
{
#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
	return _mm_cvtss_f32(_mm_add_ss(_mm_load_ss(&first), _mm_load_ss(&second)));
#else
	return first + second;
#endif
}

float RotationMultiply(float first, float second)
{
#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
	return _mm_cvtss_f32(_mm_mul_ss(_mm_load_ss(&first), _mm_load_ss(&second)));
#else
	return first * second;
#endif
}

float RotationRadians(float degrees)
{
	return (float)((double)degrees / ROTATION_HALF_TURN * M_PI);
}

void RotateVectorX(Vector &vector, float radians)
{
	if (radians == 0.0f)
		return;

	float cosine = (float)cos((double)radians);
	float sine = (float)sin((double)radians);
	float x = vector.x;
	float z = vector.z;

	vector.x = RotationAdd(RotationMultiply(z, sine), RotationMultiply(x, cosine));
	vector.z = RotationMultiply(z, cosine) - RotationMultiply(x, sine);
}

void RotateVectorY(Vector &vector, float radians)
{
	if (radians == 0.0f)
		return;

	float cosine = (float)cos((double)radians);
	float sine = (float)sin((double)radians);
	float x = vector.x;
	float y = vector.y;

	vector.x = RotationMultiply(x, cosine) - RotationMultiply(y, sine);
	vector.y = RotationAdd(RotationMultiply(x, sine), RotationMultiply(y, cosine));
}

void RotateVectorZOrdered(Vector &vector, float radians, bool zFirst)
{
	if (radians == 0.0f)
		return;

	float cosine = (float)cos((double)radians);
	float sine = (float)sin((double)radians);
	float y = vector.y;
	float z = vector.z;

	vector.y = RotationMultiply(y, cosine) - RotationMultiply(z, sine);

	// The forward path selects the z product first, including its NaN payload
	if (zFirst)
		vector.z = RotationAdd(RotationMultiply(z, cosine), RotationMultiply(y, sine));
	else
		vector.z = RotationAdd(RotationMultiply(y, sine), RotationMultiply(z, cosine));
}

void RotateVectorZ(Vector &vector, float radians)
{
	RotateVectorZOrdered(vector, radians, false);
}

void UTIL_RotateVector(Vector *vector, const Vector &angles, const Vector &offset, bool reverse)
{
	if (!vector || vector->Length() == 0.0f)
		return;

	*vector = *vector - offset;

	if (reverse)
	{
		RotateVectorY(*vector, RotationRadians(angles.y));
		RotateVectorX(*vector, RotationRadians(angles.x));
		RotateVectorZ(*vector, RotationRadians(angles.z));
	}
	else
	{
		RotateVectorZOrdered(*vector, RotationRadians(angles.z), true);
		RotateVectorX(*vector, RotationRadians(angles.x));
		RotateVectorY(*vector, RotationRadians(angles.y));
	}

	vector->x = RotationAdd(vector->x, offset.x);
	vector->y = RotationAdd(offset.y, vector->y);
	vector->z = RotationAdd(offset.z, vector->z);
}

void UTIL_RotateVectorReverse(Vector *vector, const Vector &angles, const Vector &offset)
{
	UTIL_RotateVector(vector, angles, offset, true);
}
