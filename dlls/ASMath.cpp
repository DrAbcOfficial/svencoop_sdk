#include "extdll.h"
#include "ASMath.h"
#include "CASDocumentation.h"
#include "CASMath.h"
#include "ASVector.h"

void RegisterSCScriptMath(CASDocumentation *documentation)
{
	RegisterSCScriptVector2D(documentation);
	RegisterSCScriptVector3D(documentation);
	RegisterSCScriptCMath(documentation);
}

void RegisterSCScriptCMath(CASDocumentation *documentation)
{
	RegisterSCScriptCMathType(documentation);
	documentation->RegisterGlobalProperty("Math functions instance", "CMath Math", CASMath::GetInstance());
}

void RegisterSCScriptCMathType(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Math functions", "CMath", 0, asOBJ_REF | asOBJ_NOCOUNT);

	documentation->RegisterObjectProperty("Int8 minimum value", "CMath", "const int8 INT8_MIN",
		asOFFSET(CASMath, m_i8Min));
	documentation->RegisterObjectProperty("Int16 minimum value", "CMath", "const int16 INT16_MIN",
		asOFFSET(CASMath, m_i16Min));
	documentation->RegisterObjectProperty("Int32 minimum value", "CMath", "const int32 INT32_MIN",
		asOFFSET(CASMath, m_i32Min));
	documentation->RegisterObjectProperty("Int64 minimum value", "CMath", "const int64 INT64_MIN",
		asOFFSET(CASMath, m_i64Min));
	documentation->RegisterObjectProperty("Int8 maximum value", "CMath", "const int8 INT8_MAX",
		asOFFSET(CASMath, m_i8Max));
	documentation->RegisterObjectProperty("Int16 maximum value", "CMath", "const int16 INT16_MAX",
		asOFFSET(CASMath, m_i16Max));
	documentation->RegisterObjectProperty("Int32 maximum value", "CMath", "const int32 INT32_MAX",
		asOFFSET(CASMath, m_i32Max));
	documentation->RegisterObjectProperty("Int64 maximum value", "CMath", "const int64 INT64_MAX",
		asOFFSET(CASMath, m_i64Max));
	documentation->RegisterObjectProperty("Uint8 maximum value", "CMath", "const uint8 UINT8_MAX",
		asOFFSET(CASMath, m_ui8Max));
	documentation->RegisterObjectProperty("Uint16 maximum value", "CMath", "const uint16 UINT16_MAX",
		asOFFSET(CASMath, m_ui16Max));
	documentation->RegisterObjectProperty("Uint32 maximum value", "CMath", "const uint32 UINT32_MAX",
		asOFFSET(CASMath, m_ui32Max));
	documentation->RegisterObjectProperty("Uint64 maximum value", "CMath", "const uint64 UINT64_MAX",
		asOFFSET(CASMath, m_ui64Max));
	documentation->RegisterObjectProperty("Size_t maximum value", "CMath", "const size_t SIZE_MAX",
		asOFFSET(CASMath, m_uiSizeMax));
	documentation->RegisterObjectProperty("Float minimum positive value", "CMath", "const float FLOAT_MIN",
		asOFFSET(CASMath, m_flMin));
	documentation->RegisterObjectProperty("Double minimum positive value", "CMath", "const double DOUBLE_MIN",
		asOFFSET(CASMath, m_dMin));
	documentation->RegisterObjectProperty("Float maximum positive value", "CMath", "const float FLOAT_MAX",
		asOFFSET(CASMath, m_flMax));
	documentation->RegisterObjectProperty("Double maximum positive value", "CMath", "const double DOUBLE_MAX",
		asOFFSET(CASMath, m_dMax));
	documentation->RegisterObjectProperty("PI", "CMath", "const double PI",
		asOFFSET(CASMath, m_dPi));

	documentation->RegisterObjectMethod("Returns a random integer between low and high",
		"CMath", "int32 RandomLong(int32 low, int32 high)",
		asMETHOD(CASMath, RandomLong), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns a random float between low and high",
		"CMath", "float RandomFloat(float low, float high)",
		asMETHOD(CASMath, RandomFloat), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Makes vectors",
		"CMath", "void MakeVectors(const Vector& in vecAngles)",
		asMETHOD(CASMath, MakeVectors), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Rotate vector",
		"CMath", "Vector RotateVector(Vector& in vecToRotate,const Vector& in vecAngles, const Vector& in vecOffset)",
		asMETHOD(CASMath, RotateVector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Make aim vectors",
		"CMath", "void MakeAimVectors(const Vector& in vecAngles)",
		asMETHOD(CASMath, MakeAimVectors), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Make inverted vectors",
		"CMath", "void MakeInvVectors(const Vector& in vecAngles)",
		asMETHOD(CASMath, MakeInvVectors), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Transforms a vector to a yaw value",
		"CMath", "float VecToYaw(const Vector& in vec)",
		asMETHOD(CASMath, VecToYaw), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Transforms a vector to an angle",
		"CMath", "Vector VecToAngles(const Vector& in vec)",
		asMETHOD(CASMath, VecToAngles), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clamps the given angle between 0 and 360",
		"CMath", "float AngleMod(float flAngle)",
		asMETHOD(CASMath, AngleMod), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the difference between 2 angles",
		"CMath", "float AngleDiff(float flDestAngle, float flSrcAngle)",
		asMETHOD(CASMath, AngleDiff), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Given a target and current angle value, and a rotation speed, all in degrees, returns a new angle "
		"that is either between value and target, or target, depending on whether the speed is high enough to "
		"reach that angle.",
		"CMath", "float ApproachAngle(float target, float value, float speed)",
		asMETHOD(CASMath, ApproachAngle), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the distance between 2 angles",
		"CMath", "float AngleDistance(float flNext, float flCur)",
		asMETHOD(CASMath, AngleDistance), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the smaller of the two values",
		"CMath", "uint64 min(uint64 lhs, uint64 rhs) const",
		asMETHODPR(CASMath, Min, (unsigned long long, unsigned long long) const, unsigned long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the smaller of the two values",
		"CMath", "int64 min(int64 lhs, int64 rhs) const",
		asMETHODPR(CASMath, Min, (long long, long long) const, long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the smaller of the two values",
		"CMath", "float min(float lhs, float rhs) const",
		asMETHODPR(CASMath, Min, (float, float) const, float), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the larger of the two values",
		"CMath", "uint64 max(uint64 lhs, uint64 rhs) const",
		asMETHODPR(CASMath, Max, (unsigned long long, unsigned long long) const, unsigned long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the larger of the two values",
		"CMath", "int64 max(int64 lhs, int64 rhs) const",
		asMETHODPR(CASMath, Max, (long long, long long) const, long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the larger of the two values",
		"CMath", "float max(float lhs, float rhs) const",
		asMETHODPR(CASMath, Max, (float, float) const, float), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clamps a value between a range",
		"CMath", "int clamp(int min, int max, int value) const",
		asMETHODPR(CASMath, Clamp, (long, long, long) const, long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clamps a value between a range",
		"CMath", "float clamp(float min, float max, float value) const",
		asMETHODPR(CASMath, Clamp, (float, float, float) const, float), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the float value, rounded down to the nearest whole number",
		"CMath", "float Floor(float flValue) const",
		asMETHOD(CASMath, Floor), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the float value, rounded up to the nearest whole number",
		"CMath", "float Ceil(float flValue) const",
		asMETHOD(CASMath, Ceil), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value in degrees as radians",
		"CMath", "float DegreesToRadians(float flDegrees) const",
		asMETHOD(CASMath, DegreesToRadians), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the given value in radians as degrees",
		"CMath", "float RadiansToDegrees(float flRadians) const",
		asMETHOD(CASMath, RadiansToDegrees), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Compares the given floating point variables and returns whether they are equal, considering certain "
		"variance (epsilon) between them.",
		"CMath", "bool Compare(float flLhs, float flRhs, float flEpsilon) const",
		asMETHODPR(CASMath, Compare, (float, float, float), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Compares the given floating point variables and returns whether they are equal, considering certain "
		"variance (epsilon) between them.",
		"CMath", "bool Compare(double flLhs, double flRhs, double flEpsilon) const",
		asMETHODPR(CASMath, Compare, (double, double, double), bool), asCALL_THISCALL);
}
