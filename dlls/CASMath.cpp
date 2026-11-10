#include "extdll.h"
#include "util.h"
#include "enginecallback.h"
#include "CASMath.h"
#include "VectorRotation.h"
#include <float.h>
#include <limits.h>

#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
#include <xmmintrin.h>
#endif

static const double MATH_RADIANS_PER_DEGREE = M_PI / 180.0;
static const double MATH_DEGREES_PER_RADIAN = 180.0 / M_PI;

CASMath::CASMath()
	: m_i8Min(SCHAR_MIN), m_i16Min(SHRT_MIN), m_i32Min(LONG_MIN), m_i64Min(LLONG_MIN),
	  m_i8Max(SCHAR_MAX), m_i16Max(SHRT_MAX), m_i32Max(LONG_MAX), m_i64Max(LLONG_MAX),
	  m_ui8Max(UCHAR_MAX), m_ui16Max(USHRT_MAX), m_ui32Max(ULONG_MAX), m_ui64Max(ULLONG_MAX),
	  m_uiSizeMax((size_t) -1), m_flMin(FLT_MIN), m_dMin(DBL_MIN), m_flMax(FLT_MAX),
	  m_dMax(DBL_MAX), m_dPi(M_PI)
{
}

CASMath::~CASMath()
{
}

float MathFloatReturn(float value)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	// The float return loads through x87, including signaling NaNs
	__asm
	{
		fld value
		fstp value
	}
#endif

	return value;
}

double MathFloatToDouble(float value)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	double result;

	__asm
	{
		fld value
		fstp result
	}

	return result;
#else
	return (double)value;
#endif
}

long CASMath::RandomLong(long low, long high)
{
	return RANDOM_LONG(low, high);
}

float CASMath::RandomFloat(float low, float high)
{
	return RANDOM_FLOAT(low, high);
}

void CASMath::MakeVectors(const Vector &angles)
{
	UTIL_MakeVectors(angles);
}

Vector CASMath::RotateVector(const Vector &vector, const Vector &angles, const Vector &offset)
{
	Vector result(vector);

	UTIL_RotateVector(&result, angles, offset, false);
	return result;
}

void CASMath::MakeAimVectors(const Vector &angles)
{
	UTIL_MakeAimVectors(angles);
}

void CASMath::MakeInvVectors(const Vector &angles)
{
	UTIL_MakeInvVectors(angles, gpGlobals);
}

float CASMath::VecToYaw(const Vector &vector)
{
	return UTIL_VecToYaw(vector);
}

Vector CASMath::VecToAngles(const Vector &vector)
{
	return UTIL_VecToAngles(vector);
}

float CASMath::AngleMod(float angle)
{
	return UTIL_AngleMod(angle);
}

float CASMath::AngleDiff(float destination, float source)
{
	return UTIL_AngleDiff(destination, source);
}

float CASMath::ApproachAngle(float target, float value, float speed)
{
	return UTIL_ApproachAngle(target, value, speed);
}

float CASMath::AngleDistance(float next, float current)
{
	return UTIL_AngleDistance(next, current);
}

unsigned long long CASMath::Min(unsigned long long lhs, unsigned long long rhs) const
{
	return lhs < rhs ? lhs : rhs;
}

long long CASMath::Min(long long lhs, long long rhs) const
{
	return lhs < rhs ? lhs : rhs;
}

// MINSS and MAXSS select rhs when the operands are equal or unordered
float CASMath::Min(float lhs, float rhs) const
{
#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
	return MathFloatReturn(_mm_cvtss_f32(_mm_min_ss(_mm_load_ss(&lhs), _mm_load_ss(&rhs))));
#else
	return lhs < rhs ? lhs : rhs;
#endif
}

unsigned long long CASMath::Max(unsigned long long lhs, unsigned long long rhs) const
{
	return lhs > rhs ? lhs : rhs;
}

long long CASMath::Max(long long lhs, long long rhs) const
{
	return lhs > rhs ? lhs : rhs;
}

float CASMath::Max(float lhs, float rhs) const
{
#if defined(_M_IX86) || defined(_M_X64) || defined(__SSE__)
	return MathFloatReturn(_mm_cvtss_f32(_mm_max_ss(_mm_load_ss(&lhs), _mm_load_ss(&rhs))));
#else
	return lhs > rhs ? lhs : rhs;
#endif
}

long CASMath::Clamp(long minimum, long maximum, long value) const
{
	if (value < minimum)
		return minimum;

	return value > maximum ? maximum : value;
}

float CASMath::Clamp(float minimum, float maximum, float value) const
{
	if (minimum > value)
		return MathFloatReturn(minimum);

	return Min(maximum, value);
}

float CASMath::Floor(float value) const
{
	return (float)floor(MathFloatToDouble(value));
}

float CASMath::Ceil(float value) const
{
	return (float)ceil(MathFloatToDouble(value));
}

float CASMath::DegreesToRadians(float degrees) const
{
	return (float)((double)degrees * MATH_RADIANS_PER_DEGREE);
}

float CASMath::RadiansToDegrees(float radians) const
{
	return (float)((double)radians * MATH_DEGREES_PER_RADIAN);
}

bool CASMath::Compare(float lhs, float rhs, float epsilon)
{
	return lhs >= rhs - epsilon && rhs + epsilon >= lhs;
}

bool CASMath::Compare(double lhs, double rhs, double epsilon)
{
	return lhs >= rhs - epsilon && rhs + epsilon >= lhs;
}
