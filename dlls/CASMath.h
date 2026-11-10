#ifndef CASMATH_H
#define CASMATH_H

#include "CASServerScriptGlobal.h"
#include <stddef.h>

class CASDocumentation;
class Vector;

class CASMath : public CASServerScriptGlobal<CASMath>
{
public:
	virtual ~CASMath();

	long RandomLong(long low, long high);
	float RandomFloat(float low, float high);
	void MakeVectors(const Vector &angles);
	Vector RotateVector(const Vector &vector, const Vector &angles, const Vector &offset);
	void MakeAimVectors(const Vector &angles);
	void MakeInvVectors(const Vector &angles);
	float VecToYaw(const Vector &vector);
	Vector VecToAngles(const Vector &vector);
	float AngleMod(float angle);
	float AngleDiff(float destination, float source);
	float ApproachAngle(float target, float value, float speed);
	float AngleDistance(float next, float current);

	unsigned long long Min(unsigned long long lhs, unsigned long long rhs) const;
	long long Min(long long lhs, long long rhs) const;
	float Min(float lhs, float rhs) const;
	unsigned long long Max(unsigned long long lhs, unsigned long long rhs) const;
	long long Max(long long lhs, long long rhs) const;
	float Max(float lhs, float rhs) const;
	long Clamp(long minimum, long maximum, long value) const;
	float Clamp(float minimum, float maximum, float value) const;
	float Floor(float value) const;
	float Ceil(float value) const;
	float DegreesToRadians(float degrees) const;
	float RadiansToDegrees(float radians) const;
	bool Compare(float lhs, float rhs, float epsilon);
	bool Compare(double lhs, double rhs, double epsilon);

private:
	friend void RegisterSCScriptCMathType(CASDocumentation *documentation);
	friend class CASBaseScriptGlobal<IASServerScriptGlobal, CASMath>;

	CASMath();

	const signed char			m_i8Min;
	const short					m_i16Min;
	const long					m_i32Min;
	const long long				m_i64Min;

	const signed char			m_i8Max;
	const short					m_i16Max;
	const long					m_i32Max;
	const long long				m_i64Max;

	const unsigned char			m_ui8Max;
	const unsigned short		m_ui16Max;
	const unsigned long			m_ui32Max;
	const unsigned long long	m_ui64Max;
	const size_t				m_uiSizeMax;

	const float					m_flMin;
	const double				m_dMin;
	const float					m_flMax;
	const double				m_dMax;
	const double				m_dPi;
};

#endif
