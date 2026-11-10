#ifndef CAUTOSTRING_H
#define CAUTOSTRING_H

#include <functional>

class CAutoString
{
public:
	CAutoString();
	explicit CAutoString(const char *text);
	~CAutoString();

	const char *c_str() const;
	bool operator==(const CAutoString &other) const;

private:
	CAutoString(const CAutoString &other);
	CAutoString &operator=(const CAutoString &other);

	char	*m_pszString;
};

namespace std
{
	template<>
	struct hash<CAutoString>
	{
		size_t operator()(const CAutoString &string) const;
	};
}

#endif
