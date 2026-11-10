#include "CASDateTime.h"
#include "CString.h"
#include <math.h>
#include <string.h>

long long CurrentTimestamp(void)
{
#ifdef _WIN32
	return _time64(NULL);
#else
	return time(NULL);
#endif
}

tm CalendarFromTimestamp(long long timestamp)
{
#ifdef _WIN32
	__time64_t time = timestamp;

	return *_localtime64(&time);
#else
	time_t time = (time_t)timestamp;

	return *localtime(&time);
#endif
}

long long TimestampFromCalendar(const tm &time)
{
	tm calendar = time;

#ifdef _WIN32
	return _mktime64(&calendar);
#else
	return mktime(&calendar);
#endif
}

double DifferenceBetweenTimestamps(long long end, long long begin)
{
#ifdef _WIN32
	return _difftime64(end, begin);
#else
	return difftime((time_t)end, (time_t)begin);
#endif
}

int ClampCalendarValue(int value, int minimum, int maximum)
{
	if (value > maximum)
		value = maximum;

	if (value < minimum)
		value = minimum;

	return value;
}

CASTimeDifference::CASTimeDifference() : m_flTimeDifference(0)
{
}

CASTimeDifference::CASTimeDifference(const CASTimeDifference &other)
	: m_flTimeDifference(other.m_flTimeDifference)
{
}

CASTimeDifference::CASTimeDifference(double value) : m_flTimeDifference(value)
{
}

CASTimeDifference::CASTimeDifference(const CASDateTime &time)
{
	SetTime(time);
}

CASTimeDifference::CASTimeDifference(const CASDateTime &end, const CASDateTime &begin)
{
	SetDifferenceBetween(end, begin);
}

CASTimeDifference &CASTimeDifference::operator=(const CASTimeDifference &other)
{
	if (this != &other)
		m_flTimeDifference = other.m_flTimeDifference;

	return *this;
}

CASTimeDifference &CASTimeDifference::operator=(double value)
{
	m_flTimeDifference = value;
	return *this;
}

CASTimeDifference &CASTimeDifference::operator=(const CASDateTime &time)
{
	SetTime(time);
	return *this;
}

void CASTimeDifference::SetTime(const CASDateTime &time)
{
	m_flTimeDifference = DifferenceBetweenTimestamps((long long)time.ToUnixTimestamp(), 0);
}

void CASTimeDifference::SetDifferenceBetween(const CASDateTime &end, const CASDateTime &begin)
{
	long long beginTime = (long long)begin.ToUnixTimestamp();
	long long endTime = (long long)end.ToUnixTimestamp();

	m_flTimeDifference = DifferenceBetweenTimestamps(endTime, beginTime);
}

double CASTimeDifference::GetTimeDifference(void) const
{
	return m_flTimeDifference;
}

bool CASTimeDifference::IsPositive(void) const
{
	return m_flTimeDifference >= 0;
}

int CASTimeDifference::GetSeconds(void) const
{
	return (int)fabs(m_flTimeDifference);
}

int CASTimeDifference::GetMinutes(void) const
{
	return (int)(fabs(m_flTimeDifference) / DateTimeConstants::SECONDS_PER_MINUTE);
}

int CASTimeDifference::GetHours(void) const
{
	return (int)(fabs(m_flTimeDifference) / DateTimeConstants::SECONDS_PER_HOUR);
}

int CASTimeDifference::GetDays(void) const
{
	return (int)(fabs(m_flTimeDifference) / DateTimeConstants::SECONDS_PER_DAY);
}

int CASTimeDifference::GetYears(void) const
{
	// This divides seconds directly by days per year
	return (int)(fabs(m_flTimeDifference) / DateTimeConstants::DAYS_PER_YEAR);
}

void CASTimeDifference::MakeAbsolute(void)
{
	m_flTimeDifference = fabs(m_flTimeDifference);
}

CASDateTime::CASDateTime() : m_uiMilliseconds(0)
{
	m_Time = CalendarFromTimestamp(CurrentTimestamp());
}

CASDateTime::CASDateTime(unsigned int milliseconds, int seconds, int minutes, int hour,
	int dayOfMonth, int month, int year) : m_uiMilliseconds(milliseconds)
{
	m_Time = CalendarFromTimestamp(CurrentTimestamp());
	m_Time.tm_sec = seconds;
	m_Time.tm_min = minutes;
	m_Time.tm_hour = hour;
	m_Time.tm_mday = dayOfMonth;
	m_Time.tm_mon = month;
	m_Time.tm_year = year;
	Validate();
}

CASDateTime::CASDateTime(ASUnixTime_t time) : m_uiMilliseconds(0)
{
	m_Time = CalendarFromTimestamp((long long)time);
}

CASDateTime::CASDateTime(const tm &time) : m_Time(time), m_uiMilliseconds(0)
{
}

CASDateTime::CASDateTime(const CASDateTime &other) : m_Time(other.m_Time), m_uiMilliseconds(0)
{
	Validate();
}

CASDateTime::~CASDateTime()
{
}

CASDateTime &CASDateTime::operator=(const CASDateTime &other)
{
	if (this != &other)
	{
		m_Time = other.m_Time;
		m_uiMilliseconds = other.m_uiMilliseconds;
		Validate();
	}

	return *this;
}

CASDateTime &CASDateTime::operator=(ASUnixTime_t time)
{
	SetUnixTimestamp(time);
	return *this;
}

ASUnixTime_t CASDateTime::ToUnixTimestamp(void) const
{
	return (ASUnixTime_t)TimestampFromCalendar(m_Time);
}

void CASDateTime::SetUnixTimestamp(ASUnixTime_t time)
{
	m_Time = CalendarFromTimestamp((long long)time);
}

unsigned int CASDateTime::GetMilliseconds(void) const
{
	return m_uiMilliseconds;
}

int CASDateTime::GetSeconds(void) const
{
	return m_Time.tm_sec;
}

int CASDateTime::GetMinutes(void) const
{
	return m_Time.tm_min;
}

int CASDateTime::GetHour(void) const
{
	return m_Time.tm_hour;
}

int CASDateTime::GetDayOfMonth(void) const
{
	return m_Time.tm_mday;
}

int CASDateTime::GetMonth(void) const
{
	return m_Time.tm_mon + 1;
}

int CASDateTime::GetYear(void) const
{
	return m_Time.tm_year + DateTimeConstants::YEAR_BASE;
}

int CASDateTime::GetIsDst(void) const
{
	return m_Time.tm_isdst;
}

void CASDateTime::SetMilliseconds(unsigned int milliseconds)
{
	m_uiMilliseconds = milliseconds;
}

void CASDateTime::SetSeconds(int seconds)
{
	m_Time.tm_sec = ClampCalendarValue(seconds, 0, DateTimeConstants::SECONDS_PER_MINUTE);
	Validate();
}

void CASDateTime::SetMinutes(int minutes)
{
	m_Time.tm_min = ClampCalendarValue(minutes, 0, DateTimeConstants::MINUTES_PER_HOUR - 1);
	Validate();
}

void CASDateTime::SetHour(int hour)
{
	m_Time.tm_hour = ClampCalendarValue(hour, 0, DateTimeConstants::HOURS_PER_DAY - 1);
	Validate();
}

void CASDateTime::SetDayOfMonth(int day)
{
	m_Time.tm_mday = ClampCalendarValue(day, 1, DateTimeConstants::MAX_DAYS_PER_MONTH);
	Validate();
}

void CASDateTime::SetMonth(int month)
{
	m_Time.tm_mon = ClampCalendarValue(month, 1, DateTimeConstants::MONTHS_PER_YEAR) - 1;
	Validate();
}

void CASDateTime::SetYear(int year)
{
	m_Time.tm_year = year - DateTimeConstants::YEAR_BASE;
	Validate();
}

void CASDateTime::SetIsDst(int isDst)
{
	// This setter ignores its argument and enables DST before normalizing
	m_Time.tm_isdst = 1;
	Validate();
}

int CASDateTime::Format(CString &result, const CString &format) const
{
	if (!format.Length())
	{
		result.Clear();
		return 0;
	}

	unsigned int length = DateTimeConstants::INITIAL_FORMAT_LENGTH;
	unsigned int formattedLength;

	do
	{
		result.Reserve(length, true);

		formattedLength = strftime(&result[0], result.GetCapacity(), format.Data(), &m_Time);

		if (formattedLength)
			break;

		length *= 2;
	}
	while (length <= DateTimeConstants::MAX_FORMAT_LENGTH);

	result.RecalculateLength();
	return formattedLength;
}

void CASDateTime::ToString(CString &result) const
{
	result = asctime(&m_Time);

	// Truncation keeps the stored string length from asctime
	result.Resize(result.Length() - 1, true);
}

bool CASDateTime::operator==(const CASDateTime &other) const
{
	return ToUnixTimestamp() == other.ToUnixTimestamp();
}

bool CASDateTime::operator<(const CASDateTime &other) const
{
	return ToUnixTimestamp() < other.ToUnixTimestamp();
}

bool CASDateTime::operator>(const CASDateTime &other) const
{
	return ToUnixTimestamp() > other.ToUnixTimestamp();
}

int CASDateTime::Compare(const CASDateTime &other) const
{
	long long time = (long long)ToUnixTimestamp();
	long long otherTime = (long long)other.ToUnixTimestamp();

	if (time == otherTime)
		return 0;

	return time > otherTime ? 1 : -1;
}

CASTimeDifference CASDateTime::operator-(const CASDateTime &other) const
{
	return CASTimeDifference(*this, other);
}

CASDateTime &CASDateTime::operator+=(const CASTimeDifference &difference)
{
	SetUnixTimestamp(ToUnixTimestamp() + (long long)difference.GetTimeDifference());
	return *this;
}

CASDateTime CASDateTime::operator+(const CASTimeDifference &difference) const
{
	CASTimeDifference wholeSeconds((double)(long long)difference.GetTimeDifference());
	CASDateTime result(*this);

	result += wholeSeconds;
	return result;
}

void CASDateTime::Validate(void)
{
	m_Time = CalendarFromTimestamp(TimestampFromCalendar(m_Time));
}
