#ifndef CASARGUMENT_H
#define CASARGUMENT_H

#include <angelscript.h>

enum ArgumentType_t
{
	ARGUMENT_NONE = 1,
	ARGUMENT_VOID = 2,
	ARGUMENT_PRIMITIVE = 4,
	ARGUMENT_OBJECT = 8,
	ARGUMENT_HANDLE = 16,
	ARGUMENT_ENUM = 32
};

union ArgumentValue_t
{
	asBYTE		byteValue;
	asWORD		wordValue;
	asDWORD		dwordValue;
	asQWORD		qwordValue;
	float		floatValue;
	double		doubleValue;
	void		*pointerValue;
};

class CASArgument
{
public:
	CASArgument();
	CASArgument(const CASArgument &other);
	~CASArgument();

	CASArgument &operator=(const CASArgument &other);
	void Reset(void);
	bool Set(int typeId, ArgumentType_t argumentType, const ArgumentValue_t &value, bool copyObject);
	bool Set(const CASArgument &other);
	void *GetArgumentAsPointer(void) const;

	int             m_iTypeId;
	ArgumentType_t  m_ArgumentType;
	ArgumentValue_t m_Value;
};

#endif
