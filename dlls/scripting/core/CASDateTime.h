#ifndef CASDATETIME_H
#define CASDATETIME_H

#include "scripting/core/CASBaseClass.h"
#include <time.h>

class CString;
class CASDateTime;

typedef unsigned long long ASUnixTime_t;

namespace DateTimeConstants
{
	enum
	{
		SECONDS_PER_MINUTE = 60,
		MINUTES_PER_HOUR = 60,
		HOURS_PER_DAY = 24,
		SECONDS_PER_HOUR = SECONDS_PER_MINUTE * MINUTES_PER_HOUR,
		SECONDS_PER_DAY = SECONDS_PER_HOUR * HOURS_PER_DAY,
		DAYS_PER_YEAR = 365,
		MONTHS_PER_YEAR = 12,
		MAX_DAYS_PER_MONTH = 31,
		YEAR_BASE = 1900,
		INITIAL_FORMAT_LENGTH = 512,
		MAX_FORMAT_LENGTH = 1U << 30
	};
}

class CASTimeDifference
{
public:
	CASTimeDifference();
	CASTimeDifference(const CASTimeDifference &other);
	CASTimeDifference(double value);
	CASTimeDifference(const CASDateTime &time);
	CASTimeDifference(const CASDateTime &end, const CASDateTime &begin);

	CASTimeDifference &operator=(const CASTimeDifference &other);
	CASTimeDifference &operator=(double value);
	CASTimeDifference &operator=(const CASDateTime &time);
	void SetTime(const CASDateTime &time);
	void SetDifferenceBetween(const CASDateTime &end, const CASDateTime &begin);
	double GetTimeDifference(void) const;
	bool IsPositive(void) const;
	int GetSeconds(void) const;
	int GetMinutes(void) const;
	int GetHours(void) const;
	int GetDays(void) const;
	int GetYears(void) const;
	void MakeAbsolute(void);

private:
	double	m_flTimeDifference;
};

class CASDateTime : public CASBaseClass
{
public:
	CASDateTime();
	CASDateTime(unsigned int milliseconds, int seconds, int minutes, int hour,
		int dayOfMonth, int month, int year);
	CASDateTime(ASUnixTime_t time);
	CASDateTime(const tm &time);
	CASDateTime(const CASDateTime &other);
	~CASDateTime();

	CASDateTime &operator=(const CASDateTime &other);
	CASDateTime &operator=(ASUnixTime_t time);
	CASDateTime &operator=(long time) { return *this = (ASUnixTime_t) time; }
	ASUnixTime_t ToUnixTimestamp(void) const;
	void SetUnixTimestamp(ASUnixTime_t time);
	unsigned int GetMilliseconds(void) const;
	int GetSeconds(void) const;
	int GetMinutes(void) const;
	int GetHour(void) const;
	int GetDayOfMonth(void) const;
	int GetMonth(void) const;
	int GetYear(void) const;
	int GetIsDst(void) const;
	void SetMilliseconds(unsigned int milliseconds);
	void SetSeconds(int seconds);
	void SetMinutes(int minutes);
	void SetHour(int hour);
	void SetDayOfMonth(int day);
	void SetMonth(int month);
	void SetYear(int year);
	void SetIsDst(int isDst);
	int Format(CString &result, const CString &format) const;
	void ToString(CString &result) const;
	bool operator==(const CASDateTime &other) const;
	bool operator<(const CASDateTime &other) const;
	bool operator>(const CASDateTime &other) const;
	int Compare(const CASDateTime &other) const;
	CASTimeDifference operator-(const CASDateTime &other) const;
	CASDateTime &operator+=(const CASTimeDifference &difference);
	CASDateTime operator+(const CASTimeDifference &difference) const;
	void Validate(void);

private:
	tm				m_Time;
	unsigned int	m_uiMilliseconds;
};

#endif
