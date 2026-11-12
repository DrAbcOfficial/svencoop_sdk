#include "scripting/api/ASDateTime.h"
#include "scripting/core/CASDateTime.h"
#include "scripting/core/CASDocumentation.h"
#include "PortableTime.h"
#include <new>

void DefaultConstructTimeDifference(void *memory)
{
	new (memory) CASTimeDifference;
}

void CopyConstructTimeDifference(const CASTimeDifference &other, void *memory)
{
	new (memory) CASTimeDifference(other);
}

void DoubleConstructTimeDifference(double value, void *memory)
{
	new (memory) CASTimeDifference(value);
}

void RangeConstructTimeDifference(const CASDateTime &end, const CASDateTime &begin, void *memory)
{
	new (memory) CASTimeDifference(end, begin);
}

void TimeConstructTimeDifference(const CASDateTime &time, void *memory)
{
	new (memory) CASTimeDifference(time);
}

void DestructTimeDifference(CASTimeDifference *time)
{
	time->~CASTimeDifference();
}

void DefaultConstructDateTime(void *memory)
{
	new (memory) CASDateTime;
}

void InitConstructDateTime(unsigned int milliseconds, int seconds, int minutes, int hour,
	int dayOfMonth, int month, int year, void *memory)
{
	new (memory) CASDateTime(milliseconds, seconds, minutes, hour, dayOfMonth, month, year);
}

void UnixConstructDateTime(ASUnixTime_t time, void *memory)
{
	new (memory) CASDateTime(time);
}

void CopyConstructDateTime(const CASDateTime &other, void *memory)
{
	new (memory) CASDateTime(other);
}

void DestructDateTime(void *memory)
{
	((CASDateTime *)memory)->~CASDateTime();
}

ASUnixTime_t UnixTimestamp(void)
{
	return (ASUnixTime_t) PortableTimeNow();
}

void RegisterSCScriptTimeDifference(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Represents the difference between 2 times.", "TimeDifference", sizeof(CASTimeDifference),
		asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);
	documentation->RegisterObjectBehaviour("Default constructor", "TimeDifference",
		asBEHAVE_CONSTRUCT, "void TimeDifference()",
		asFUNCTION(DefaultConstructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructor", "TimeDifference",
		asBEHAVE_CONSTRUCT, "void TimeDifference(const TimeDifference& in other)",
		asFUNCTION(CopyConstructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Sets the time difference to the given value, in seconds", "TimeDifference",
		asBEHAVE_CONSTRUCT, "void TimeDifference(double flValue)",
		asFUNCTION(DoubleConstructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Sets the time difference to the range between beginning and end. If begin is later than end, is negative.", "TimeDifference",
		asBEHAVE_CONSTRUCT, "void TimeDifference(const DateTime& in end, const DateTime& in begin)",
		asFUNCTION(RangeConstructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Sets the time difference to the range between 0 and time.", "TimeDifference",
		asBEHAVE_CONSTRUCT, "void TimeDifference(const DateTime& in time)",
		asFUNCTION(TimeConstructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Destructor", "TimeDifference",
		asBEHAVE_DESTRUCT, "void DestructTimeDifference()",
		asFUNCTION(DestructTimeDifference), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator", "TimeDifference",
		"TimeDifference& opAssign(const TimeDifference& in other)",
		asMETHODPR(CASTimeDifference, operator=, (const CASTimeDifference &), CASTimeDifference &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Assignment operator", "TimeDifference",
		"TimeDifference& opAssign(double timeDiff)",
		asMETHODPR(CASTimeDifference, operator=, (double), CASTimeDifference &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Assignment operator", "TimeDifference",
		"TimeDifference& opAssign( const DateTime& in time)",
		asMETHODPR(CASTimeDifference, operator=, (const CASDateTime &), CASTimeDifference &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference", "TimeDifference",
		"double GetTimeDifference() const",
		asMETHOD(CASTimeDifference, GetTimeDifference), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the time difference is positive.", "TimeDifference",
		"bool IsPositive() const",
		asMETHOD(CASTimeDifference, IsPositive), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference in seconds.", "TimeDifference",
		"int GetSeconds() const",
		asMETHOD(CASTimeDifference, GetSeconds), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference in minutes.", "TimeDifference",
		"int GetMinutes() const",
		asMETHOD(CASTimeDifference, GetMinutes), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference in hours.", "TimeDifference",
		"int GetHours() const",
		asMETHOD(CASTimeDifference, GetHours), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference in days.", "TimeDifference",
		"int GetDays() const",
		asMETHOD(CASTimeDifference, GetDays), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the time difference in years.", "TimeDifference",
		"int GetYears() const",
		asMETHOD(CASTimeDifference, GetYears), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Makes the time difference absolute.", "TimeDifference",
		"void MakeAbsolute()",
		asMETHOD(CASTimeDifference, MakeAbsolute), asCALL_THISCALL);

	// This two-argument declaration is bound to MakeAbsolute(), despite the calling-convention mismatch
	documentation->RegisterObjectMethod("Sets the time difference to the range between beginning and end. If begin is later than end, is negative.", "TimeDifference",
		"void SetDifferenceBetween(const DateTime& in end, const DateTime& in begin)",
		asMETHOD(CASTimeDifference, MakeAbsolute), asCALL_THISCALL);

	documentation->RegisterObjectMethod("Sets the time difference to the range between 0 and time.", "TimeDifference",
		"void SetTime(const DateTime& in time)",
		asMETHOD(CASTimeDifference, SetTime), asCALL_THISCALL);
}

void RegisterSCScriptDateTime(CASDocumentation *documentation)
{
	documentation->RegisterTypedef("Unix timestamp type.", "time_t", "uint64");
	documentation->RegisterObjectType("DateTime class.\nNote: milliseconds default to 0 due to limited accuracy.", "DateTime", sizeof(CASDateTime),
		asOBJ_VALUE | asOBJ_APP_CLASS_CDAK);

	RegisterSCScriptTimeDifference(documentation);

	documentation->RegisterObjectBehaviour("Default constructor.", "DateTime",
		asBEHAVE_CONSTRUCT, "void DateTime()",
		asFUNCTION(DefaultConstructDateTime), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Init constructor.", "DateTime",
		asBEHAVE_CONSTRUCT, "void DateTime(uint uiMilliseconds, int iSeconds, int iMinutes, int iHour, int iDayOfMonth, int iMonth, int iYear)",
		asFUNCTION(InitConstructDateTime), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Unix timestamp constructor.", "DateTime",
		asBEHAVE_CONSTRUCT, "void DateTime(time_t time)",
		asFUNCTION(UnixConstructDateTime), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructor.", "DateTime",
		asBEHAVE_CONSTRUCT, "void DateTime(const DateTime& in ref)",
		asFUNCTION(CopyConstructDateTime), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Destructor.", "DateTime",
		asBEHAVE_DESTRUCT, "void DestructDateTime()",
		asFUNCTION(DestructDateTime), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Assignment operator.", "DateTime",
		"DateTime& opAssign(const DateTime& in ref)",
		asMETHODPR(CASDateTime, operator=, (const CASDateTime &), CASDateTime &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Assignment operator.", "DateTime",
		"DateTime& opAssign(time_t time)",
		asMETHODPR(CASDateTime, operator=, (ASUnixTime_t), CASDateTime &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get time as a unix timestamp.", "DateTime",
		"time_t ToUnixTimestamp() const",
		asMETHOD(CASDateTime, ToUnixTimestamp), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Sets time as a unix timestamp.", "DateTime",
		"void SetUnixTimestamp(time_t time)",
		asMETHOD(CASDateTime, SetUnixTimestamp), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get milliseconds.", "DateTime",
		"uint GetMilliseconds() const",
		asMETHOD(CASDateTime, GetMilliseconds), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get seconds after the minute including leap second (0-60).", "DateTime",
		"int GetSeconds() const",
		asMETHOD(CASDateTime, GetSeconds), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get minutes after the hour (0-59).", "DateTime",
		"int GetMinutes() const",
		asMETHOD(CASDateTime, GetMinutes), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get hours since midnight (0-23).", "DateTime",
		"int GetHour() const",
		asMETHOD(CASDateTime, GetHour), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get day of the month (1-31).", "DateTime",
		"int GetDayOfMonth() const",
		asMETHOD(CASDateTime, GetDayOfMonth), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get month of the year (1-12).", "DateTime",
		"int GetMonth() const",
		asMETHOD(CASDateTime, GetMonth), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get year.", "DateTime",
		"int GetYear() const",
		asMETHOD(CASDateTime, GetYear), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Get daylight savings active.", "DateTime",
		"int GetIsDst() const",
		asMETHOD(CASDateTime, GetIsDst), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set milliseconds.", "DateTime",
		"void SetMilliseconds(uint uiMilliseconds)",
		asMETHOD(CASDateTime, SetMilliseconds), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set seconds after the minute including leap second (0-60).", "DateTime",
		"void SetSeconds(int iSeconds)",
		asMETHOD(CASDateTime, SetSeconds), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set minutes after the hour (0-59).", "DateTime",
		"void SetMinutes(int iMinutes)",
		asMETHOD(CASDateTime, SetMinutes), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set hours since midnight (0-23).", "DateTime",
		"void SetHour(int iHours)",
		asMETHOD(CASDateTime, SetHour), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set day of the month (1-31).", "DateTime",
		"void SetDayOfMonth(int iDay)",
		asMETHOD(CASDateTime, SetDayOfMonth), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set month of the year (1-12).", "DateTime",
		"void SetMonth(int iMonth)",
		asMETHOD(CASDateTime, SetMonth), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set year.", "DateTime",
		"void SetYear(int iYear)",
		asMETHOD(CASDateTime, SetYear), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Set daylight savings active.", "DateTime",
		"void SetIsDst(int iIsDst)",
		asMETHOD(CASDateTime, SetIsDst), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Formats this datetime as a string.\nReturns the length of the destination string, or 0 if the buffer was not large enough.", "DateTime",
		"int Format(string& out szResult, const string& in szFormat) const",
		asMETHOD(CASDateTime, Format), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns a string representation of this datetime.", "DateTime",
		"void ToString(string& out szResult) const",
		asMETHOD(CASDateTime, ToString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether these two DateTimes represent the same time.", "DateTime",
		"bool opEquals(const DateTime& in other) const",
		asMETHOD(CASDateTime, operator==), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Comparison operator.", "DateTime",
		"int opCmp(const DateTime& in other) const",
		asMETHOD(CASDateTime, Compare), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Subtracts the given DateTime from this one, resulting in a TimeDifference object.", "DateTime",
		"TimeDifference opSub(const DateTime& in other) const",
		asMETHOD(CASDateTime, operator-), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Adds the given TimeDifference to this one.", "DateTime",
		"DateTime& opAddAssign(const TimeDifference& in diffTime)",
		asMETHOD(CASDateTime, operator+=), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Adds this DateTime and the given TimeDifferene together, returning a copy containing the new time.", "DateTime",
		"DateTime opAdd(const TimeDifference& in diffTime) const",
		asMETHOD(CASDateTime, operator+), asCALL_THISCALL);
	documentation->RegisterGlobalFunction("Gets the current time as a unix timestamp.", "time_t UnixTimestamp()",
		asFUNCTION(UnixTimestamp), asCALL_CDECL);
}
