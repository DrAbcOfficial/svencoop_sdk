#ifndef CSCRIPTDICTIONARY_H
#define CSCRIPTDICTIONARY_H

#include <angelscript.h>
#include "CStringHash.h"
#include <unordered_map>

#ifndef AS_USE_STLNAMES
#define AS_USE_STLNAMES 0
#endif

typedef CString dictKey_t;

BEGIN_AS_NAMESPACE
class CScriptDictValue;
END_AS_NAMESPACE

typedef std::unordered_map<dictKey_t, AS_NAMESPACE_QUALIFIER CScriptDictValue> dictMap_t;

BEGIN_AS_NAMESPACE

class CScriptArray;
class CScriptDictionary;

class CScriptDictValue
{
public:
	CScriptDictValue();
	CScriptDictValue(asIScriptEngine *engine, void *value, int typeId);

	// C++ owners must release stored objects with FreeValue before destruction
	~CScriptDictValue();

	void Set(asIScriptEngine *engine, void *value, int typeId);
	void Set(asIScriptEngine *engine, const asINT64 &value);
	void Set(asIScriptEngine *engine, const double &value);
	void Set(asIScriptEngine *engine, CScriptDictValue &value);

	bool Get(asIScriptEngine *engine, void *value, int typeId) const;
	bool Get(asIScriptEngine *engine, asINT64 &value) const;
	bool Get(asIScriptEngine *engine, double &value) const;

	const void *GetAddressOfValue() const;
	int GetTypeId() const;
	void FreeValue(asIScriptEngine *engine);
	void EnumReferences(asIScriptEngine *engine);

protected:
	friend class CScriptDictionary;

	union
	{
		asINT64	m_valueInt;
		double	m_valueFlt;
		void	*m_valueObj;
	};
	int	m_typeId;
};

class CScriptDictionary
{
public:
	static CScriptDictionary *Create(asIScriptEngine *engine);
	static CScriptDictionary *Create(asBYTE *buffer);

	void AddRef() const;
	void Release() const;
	CScriptDictionary &operator=(const CScriptDictionary &other);

	void Set(const dictKey_t &key, void *value, int typeId);
	void Set(const dictKey_t &key, const asINT64 &value);
	void Set(const dictKey_t &key, const double &value);

	bool Get(const dictKey_t &key, void *value, int typeId) const;
	bool Get(const dictKey_t &key, asINT64 &value) const;
	bool Get(const dictKey_t &key, double &value) const;

	CScriptDictValue *operator[](const dictKey_t &key);
	const CScriptDictValue *operator[](const dictKey_t &key) const;

	int GetTypeId(const dictKey_t &key) const;
	bool Exists(const dictKey_t &key) const;
	bool IsEmpty() const;
	asUINT GetSize() const;
	bool Delete(const dictKey_t &key);
	void DeleteAll();
	CScriptArray *GetKeys() const;

	class CIterator
	{
	public:
		void operator++();
		void operator++(int);
		CIterator &operator*();

		bool operator==(const CIterator &other) const;
		bool operator!=(const CIterator &other) const;

		const dictKey_t &GetKey() const;
		int GetTypeId() const;
		bool GetValue(asINT64 &value) const;
		bool GetValue(double &value) const;
		bool GetValue(void *value, int typeId) const;
		const void *GetAddressOfValue() const;

	protected:
		friend class CScriptDictionary;

		CIterator(const CScriptDictionary &dict, dictMap_t::const_iterator it);
		CIterator &operator=(const CIterator &)
		{
			return *this;
		}

		dictMap_t::const_iterator	m_it;
		const CScriptDictionary		&m_dict;
	};

	CIterator begin() const;
	CIterator end() const;
	CIterator find(const dictKey_t &key) const;

	int GetRefCount();
	void SetGCFlag();
	bool GetGCFlag();
	void EnumReferences(asIScriptEngine *engine);
	void ReleaseAllReferences(asIScriptEngine *engine);

protected:
	CScriptDictionary(asIScriptEngine *engine);
	CScriptDictionary(asBYTE *buffer);
	virtual ~CScriptDictionary();

	void Init(asIScriptEngine *engine);

	asIScriptEngine	*engine;
	mutable int		refCount;
	mutable bool	gcFlag;
	dictMap_t		dict;
};

void RegisterScriptDictionary(asIScriptEngine *engine);
void RegisterScriptDictionary_Native(asIScriptEngine *engine);
void RegisterScriptDictionary_Generic(asIScriptEngine *engine);

END_AS_NAMESPACE

#endif
