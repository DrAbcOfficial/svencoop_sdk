#include <new>
#include <string.h>
#include "scripting/core/CScriptDictionary.h"
#include "../../../external/angelscript/sdk/add_on/scriptarray/scriptarray.h"

BEGIN_AS_NAMESPACE

const asPWORD DICTIONARY_CACHE = 1003;

struct SDictionaryCache
{
	asITypeInfo	*dictType;
	asITypeInfo	*arrayType;
	asITypeInfo	*keyType;

	static void Setup(asIScriptEngine *engine)
	{
		SDictionaryCache *cache = reinterpret_cast<SDictionaryCache *>(engine->GetUserData(DICTIONARY_CACHE));

		if (cache == NULL)
		{
			cache = new SDictionaryCache;
			engine->SetUserData(cache, DICTIONARY_CACHE);
			engine->SetEngineUserDataCleanupCallback(SDictionaryCache::Cleanup, DICTIONARY_CACHE);

			cache->dictType = engine->GetTypeInfoByName("dictionary");
			cache->arrayType = engine->GetTypeInfoByDecl("array<string>");
			cache->keyType = engine->GetTypeInfoByDecl("string");
		}
	}

	static void Cleanup(asIScriptEngine *engine)
	{
		SDictionaryCache *cache = reinterpret_cast<SDictionaryCache *>(engine->GetUserData(DICTIONARY_CACHE));

		if (cache)
			delete cache;
	}
};

CScriptDictionary *CScriptDictionary::Create(asIScriptEngine *engine)
{
	CScriptDictionary *obj = (CScriptDictionary *)asAllocMem(sizeof(CScriptDictionary));

	new (obj) CScriptDictionary(engine);
	return obj;
}

CScriptDictionary *CScriptDictionary::Create(asBYTE *buffer)
{
	CScriptDictionary *obj = (CScriptDictionary *)asAllocMem(sizeof(CScriptDictionary));

	new (obj) CScriptDictionary(buffer);
	return obj;
}

CScriptDictionary::CScriptDictionary(asIScriptEngine *engine)
{
	Init(engine);
}

void CScriptDictionary::Init(asIScriptEngine *e)
{
	refCount = 1;
	gcFlag = false;

	engine = e;

	SDictionaryCache *cache = reinterpret_cast<SDictionaryCache *>(engine->GetUserData(DICTIONARY_CACHE));

	engine->NotifyGarbageCollectorOfNewObject(this, cache->dictType);
}

CScriptDictionary::CScriptDictionary(asBYTE *buffer)
{
	asIScriptContext *ctx = asGetActiveContext();

	Init(ctx->GetEngine());

	SDictionaryCache &cache = *reinterpret_cast<SDictionaryCache *>(engine->GetUserData(DICTIONARY_CACHE));
	bool keyAsRef = cache.keyType->GetFlags() & asOBJ_REF ? true : false;

	asUINT length = *(asUINT *)buffer;

	buffer += sizeof(asUINT);

	while (length--)
	{
		const asPWORD alignment = sizeof(asDWORD);

		if (asPWORD(buffer) & (alignment - 1))
			buffer += alignment - (asPWORD(buffer) & (alignment - 1));

		dictKey_t name;

		if (keyAsRef)
		{
			name = **(dictKey_t **)buffer;
			buffer += sizeof(dictKey_t *);
		}
		else
		{
			name = *(dictKey_t *)buffer;
			buffer += sizeof(dictKey_t);
		}

		int typeId = *(int *)buffer;

		buffer += sizeof(int);

		void *ref = (void *)buffer;

		if (typeId >= asTYPEID_INT8 && typeId <= asTYPEID_DOUBLE)
		{
			asINT64 i64;
			double d;

			switch (typeId)
			{
			case asTYPEID_INT8:
				i64 = *(char *)ref;
				break;
			case asTYPEID_INT16:
				i64 = *(short *)ref;
				break;
			case asTYPEID_INT32:
				i64 = *(int *)ref;
				break;
			case asTYPEID_INT64:
				i64 = *(asINT64 *)ref;
				break;
			case asTYPEID_UINT8:
				i64 = *(unsigned char *)ref;
				break;
			case asTYPEID_UINT16:
				i64 = *(unsigned short *)ref;
				break;
			case asTYPEID_UINT32:
				i64 = *(unsigned int *)ref;
				break;
			case asTYPEID_UINT64:
				i64 = *(asINT64 *)ref;
				break;
			case asTYPEID_FLOAT:
				d = *(float *)ref;
				break;
			case asTYPEID_DOUBLE:
				d = *(double *)ref;
				break;
			}

			if (typeId >= asTYPEID_FLOAT)
				Set(name, d);
			else
				Set(name, i64);
		}
		else
		{
			if ((typeId & asTYPEID_MASK_OBJECT) && !(typeId & asTYPEID_OBJHANDLE) &&
				(engine->GetTypeInfoById(typeId)->GetFlags() & asOBJ_REF))
			{
				ref = *(void **)ref;
			}

			Set(name, ref, typeId);
		}

		if (typeId & asTYPEID_MASK_OBJECT)
		{
			asITypeInfo *ti = engine->GetTypeInfoById(typeId);

			if (ti->GetFlags() & asOBJ_VALUE)
				buffer += ti->GetSize();
			else
				buffer += sizeof(void *);
		}
		else if (typeId == asTYPEID_VOID)
		{
			buffer += sizeof(void *);
		}
		else
		{
			buffer += engine->GetSizeOfPrimitiveType(typeId);
		}
	}
}

CScriptDictionary::~CScriptDictionary()
{
	DeleteAll();
}

void CScriptDictionary::AddRef() const
{
	gcFlag = false;
	asAtomicInc(refCount);
}

void CScriptDictionary::Release() const
{
	gcFlag = false;
	if (asAtomicDec(refCount) == 0)
	{
		this->~CScriptDictionary();
		asFreeMem(const_cast<CScriptDictionary *>(this));
	}
}

int CScriptDictionary::GetRefCount()
{
	return refCount;
}

void CScriptDictionary::SetGCFlag()
{
	gcFlag = true;
}

bool CScriptDictionary::GetGCFlag()
{
	return gcFlag;
}

void CScriptDictionary::EnumReferences(asIScriptEngine *inEngine)
{
	dictMap_t::iterator it;

	for (it = dict.begin(); it != dict.end(); it++)
	{
		if (it->second.m_typeId & asTYPEID_MASK_OBJECT)
		{
			asITypeInfo *subType = engine->GetTypeInfoById(it->second.m_typeId);

			if ((subType->GetFlags() & asOBJ_VALUE) && (subType->GetFlags() & asOBJ_GC))
			{
				engine->ForwardGCEnumReferences(it->second.m_valueObj, subType);
			}
			else
			{
				inEngine->GCEnumCallback(it->second.m_valueObj);
			}
		}
	}
}

void CScriptDictionary::ReleaseAllReferences(asIScriptEngine * /*engine*/)
{
	DeleteAll();
}

CScriptDictionary &CScriptDictionary::operator=(const CScriptDictionary &other)
{
	DeleteAll();

	dictMap_t::const_iterator it;

	for (it = other.dict.begin(); it != other.dict.end(); it++)
	{
		if (it->second.m_typeId & asTYPEID_OBJHANDLE)
			Set(it->first, (void *)&it->second.m_valueObj, it->second.m_typeId);
		else if (it->second.m_typeId & asTYPEID_MASK_OBJECT)
			Set(it->first, (void *)it->second.m_valueObj, it->second.m_typeId);
		else
			Set(it->first, (void *)&it->second.m_valueInt, it->second.m_typeId);
	}

	return *this;
}

CScriptDictValue *CScriptDictionary::operator[](const dictKey_t &key)
{
	return &dict[key];
}

const CScriptDictValue *CScriptDictionary::operator[](const dictKey_t &key) const
{
	dictMap_t::const_iterator it = dict.find(key);

	if (it != dict.end())
		return &it->second;

	asIScriptContext *ctx = asGetActiveContext();

	if (ctx)
		ctx->SetException("Invalid access to non-existing value");

	return NULL;
}

void CScriptDictionary::Set(const dictKey_t &key, void *value, int typeId)
{
	dictMap_t::iterator it = dict.find(key);

	if (it == dict.end())
		it = dict.insert(dictMap_t::value_type(key, CScriptDictValue())).first;

	it->second.Set(engine, value, typeId);
}

void CScriptDictionary::Set(const dictKey_t &key, const asINT64 &value)
{
	Set(key, const_cast<asINT64 *>(&value), asTYPEID_INT64);
}

void CScriptDictionary::Set(const dictKey_t &key, const double &value)
{
	Set(key, const_cast<double *>(&value), asTYPEID_DOUBLE);
}

bool CScriptDictionary::Get(const dictKey_t &key, void *value, int typeId) const
{
	dictMap_t::const_iterator it = dict.find(key);

	if (it != dict.end())
		return it->second.Get(engine, value, typeId);

	return false;
}

int CScriptDictionary::GetTypeId(const dictKey_t &key) const
{
	dictMap_t::const_iterator it = dict.find(key);

	if (it != dict.end())
		return it->second.m_typeId;

	return asERROR;
}

bool CScriptDictionary::Get(const dictKey_t &key, asINT64 &value) const
{
	return Get(key, &value, asTYPEID_INT64);
}

bool CScriptDictionary::Get(const dictKey_t &key, double &value) const
{
	return Get(key, &value, asTYPEID_DOUBLE);
}

bool CScriptDictionary::Exists(const dictKey_t &key) const
{
	dictMap_t::const_iterator it = dict.find(key);

	if (it != dict.end())
		return true;

	return false;
}

bool CScriptDictionary::IsEmpty() const
{
	if (dict.size() == 0)
		return true;

	return false;
}

asUINT CScriptDictionary::GetSize() const
{
	return asUINT(dict.size());
}

bool CScriptDictionary::Delete(const dictKey_t &key)
{
	dictMap_t::iterator it = dict.find(key);

	if (it != dict.end())
	{
		it->second.FreeValue(engine);
		dict.erase(it);
		return true;
	}

	return false;
}

void CScriptDictionary::DeleteAll()
{
	dictMap_t::iterator it;

	for (it = dict.begin(); it != dict.end(); it++)
		it->second.FreeValue(engine);

	dict.clear();
}

CScriptArray *CScriptDictionary::GetKeys() const
{
	SDictionaryCache *cache = reinterpret_cast<SDictionaryCache *>(engine->GetUserData(DICTIONARY_CACHE));
	asITypeInfo *ti = cache->arrayType;

	CScriptArray *array = CScriptArray::Create(ti, asUINT(dict.size()));
	long current = -1;
	dictMap_t::const_iterator it;

	for (it = dict.begin(); it != dict.end(); it++)
	{
		current++;
		*(dictKey_t *)array->At(current) = it->first;
	}

	return array;
}

void ScriptDictionaryFactory_Generic(asIScriptGeneric *gen)
{
	*(CScriptDictionary **)gen->GetAddressOfReturnLocation() = CScriptDictionary::Create(gen->GetEngine());
}

void ScriptDictionaryListFactory_Generic(asIScriptGeneric *gen)
{
	asBYTE *buffer = (asBYTE *)gen->GetArgAddress(0);

	*(CScriptDictionary **)gen->GetAddressOfReturnLocation() = CScriptDictionary::Create(buffer);
}

void ScriptDictionaryAddRef_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();

	dict->AddRef();
}

void ScriptDictionaryRelease_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();

	dict->Release();
}

void ScriptDictionaryAssign_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	CScriptDictionary *other = *(CScriptDictionary **)gen->GetAddressOfArg(0);

	*dict = *other;
	*(CScriptDictionary **)gen->GetAddressOfReturnLocation() = dict;
}

void ScriptDictionarySet_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);
	int typeId = gen->GetArgTypeId(1);

	dict->Set(*key, ref, typeId);
}

void ScriptDictionarySetInt_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);

	dict->Set(*key, *(asINT64 *)ref);
}

void ScriptDictionarySetFlt_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);

	dict->Set(*key, *(double *)ref);
}

void ScriptDictionaryGet_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);
	int typeId = gen->GetArgTypeId(1);

	*(bool *)gen->GetAddressOfReturnLocation() = dict->Get(*key, ref, typeId);
}

void ScriptDictionaryGetInt_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);

	*(bool *)gen->GetAddressOfReturnLocation() = dict->Get(*key, *(asINT64 *)ref);
}

void ScriptDictionaryGetFlt_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	void *ref = *(void **)gen->GetAddressOfArg(1);

	*(bool *)gen->GetAddressOfReturnLocation() = dict->Get(*key, *(double *)ref);
}

void ScriptDictionaryExists_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);
	bool ret = dict->Exists(*key);

	*(bool *)gen->GetAddressOfReturnLocation() = ret;
}

void ScriptDictionaryIsEmpty_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	bool ret = dict->IsEmpty();

	*(bool *)gen->GetAddressOfReturnLocation() = ret;
}

void ScriptDictionaryGetSize_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	asUINT ret = dict->GetSize();

	*(asUINT *)gen->GetAddressOfReturnLocation() = ret;
}

void ScriptDictionaryDelete_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);

	*(bool *)gen->GetAddressOfReturnLocation() = dict->Delete(*key);
}

void ScriptDictionaryDeleteAll_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *dict = (CScriptDictionary *)gen->GetObject();

	dict->DeleteAll();
}

void ScriptDictionaryGetRefCount_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();

	*(int *)gen->GetAddressOfReturnLocation() = self->GetRefCount();
}

void ScriptDictionarySetGCFlag_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();

	self->SetGCFlag();
}

void ScriptDictionaryGetGCFlag_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();

	*(bool *)gen->GetAddressOfReturnLocation() = self->GetGCFlag();
}

void ScriptDictionaryEnumReferences_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();
	asIScriptEngine *engine = *(asIScriptEngine **)gen->GetAddressOfArg(0);

	self->EnumReferences(engine);
}

void ScriptDictionaryReleaseAllReferences_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();
	asIScriptEngine *engine = *(asIScriptEngine **)gen->GetAddressOfArg(0);

	self->ReleaseAllReferences(engine);
}

void CScriptDictionaryGetKeys_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();

	*(CScriptArray **)gen->GetAddressOfReturnLocation() = self->GetKeys();
}

void CScriptDictionary_opIndex_Generic(asIScriptGeneric *gen)
{
	CScriptDictionary *self = (CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);

	*(CScriptDictValue **)gen->GetAddressOfReturnLocation() = self->operator[](*key);
}

void CScriptDictionary_opIndex_const_Generic(asIScriptGeneric *gen)
{
	const CScriptDictionary *self = (const CScriptDictionary *)gen->GetObject();
	dictKey_t *key = *(dictKey_t **)gen->GetAddressOfArg(0);

	*(const CScriptDictValue **)gen->GetAddressOfReturnLocation() = self->operator[](*key);
}

CScriptDictValue::CScriptDictValue()
{
	m_valueObj = NULL;
	m_typeId = asTYPEID_VOID;
}

CScriptDictValue::CScriptDictValue(asIScriptEngine *engine, void *value, int typeId)
{
	m_valueObj = NULL;
	m_typeId = asTYPEID_VOID;

	Set(engine, value, typeId);
}

CScriptDictValue::~CScriptDictValue()
{
	if (m_valueObj && m_typeId)
	{
		asIScriptContext *ctx = asGetActiveContext();

		if (ctx)
			FreeValue(ctx->GetEngine());
	}
}

void CScriptDictValue::FreeValue(asIScriptEngine *engine)
{
	if (m_typeId & asTYPEID_MASK_OBJECT)
	{
		engine->ReleaseScriptObject(m_valueObj, engine->GetTypeInfoById(m_typeId));
		m_valueObj = NULL;
		m_typeId = asTYPEID_VOID;
	}
}

void CScriptDictValue::EnumReferences(asIScriptEngine *inEngine)
{
	if (m_valueObj)
		inEngine->GCEnumCallback(m_valueObj);

	if (m_typeId)
		inEngine->GCEnumCallback(inEngine->GetTypeInfoById(m_typeId));
}

void CScriptDictValue::Set(asIScriptEngine *engine, void *value, int typeId)
{
	FreeValue(engine);

	m_typeId = typeId;

	if (typeId & asTYPEID_OBJHANDLE)
	{
		m_valueObj = *(void **)value;
		engine->AddRefScriptObject(m_valueObj, engine->GetTypeInfoById(typeId));
	}
	else if (typeId & asTYPEID_MASK_OBJECT)
	{
		m_valueObj = engine->CreateScriptObjectCopy(value, engine->GetTypeInfoById(typeId));
		if (m_valueObj == NULL)
		{
			asIScriptContext *ctx = asGetActiveContext();

			if (ctx)
				ctx->SetException("Cannot create copy of object");
		}
	}
	else
	{
		int size = engine->GetSizeOfPrimitiveType(typeId);

		memmove(&m_valueInt, value, size);
	}
}

void CScriptDictValue::Set(asIScriptEngine *engine, CScriptDictValue &value)
{
	if (value.m_typeId & asTYPEID_OBJHANDLE)
		Set(engine, (void *)&value.m_valueObj, value.m_typeId);
	else if (value.m_typeId & asTYPEID_MASK_OBJECT)
		Set(engine, (void *)value.m_valueObj, value.m_typeId);
	else
		Set(engine, (void *)&value.m_valueInt, value.m_typeId);
}

void CScriptDictValue::Set(asIScriptEngine *engine, const asINT64 &value)
{
	Set(engine, const_cast<asINT64 *>(&value), asTYPEID_INT64);
}

void CScriptDictValue::Set(asIScriptEngine *engine, const double &value)
{
	Set(engine, const_cast<double *>(&value), asTYPEID_DOUBLE);
}

bool CScriptDictValue::Get(asIScriptEngine *engine, void *value, int typeId) const
{
	if (typeId & asTYPEID_OBJHANDLE)
	{
		if (m_typeId & asTYPEID_MASK_OBJECT)
		{
			if ((m_typeId & asTYPEID_HANDLETOCONST) && !(typeId & asTYPEID_HANDLETOCONST))
				return false;

			engine->RefCastObject(
				m_valueObj, engine->GetTypeInfoById(m_typeId), engine->GetTypeInfoById(typeId), reinterpret_cast<void **>(value));

			return true;
		}
	}
	else if (typeId & asTYPEID_MASK_OBJECT)
	{
		bool isCompatible = false;

		if ((m_typeId & ~(asTYPEID_OBJHANDLE | asTYPEID_HANDLETOCONST)) == typeId && m_valueObj != NULL)
			isCompatible = true;

		if (isCompatible)
		{
			engine->AssignScriptObject(value, m_valueObj, engine->GetTypeInfoById(typeId));

			return true;
		}
	}
	else
	{
		if (m_typeId == typeId)
		{
			int size = engine->GetSizeOfPrimitiveType(typeId);

			memmove(value, &m_valueInt, size);
			return true;
		}

		if (typeId == asTYPEID_DOUBLE)
		{
			if (m_typeId == asTYPEID_INT64)
				*(double *)value = double(m_valueInt);
			else if (m_typeId == asTYPEID_BOOL)
			{
				char localValue;

				memmove(&localValue, &m_valueInt, sizeof(char));
				*(double *)value = localValue ? 1.0 : 0.0;
			}
			else if (m_typeId > asTYPEID_DOUBLE && (m_typeId & asTYPEID_MASK_OBJECT) == 0)
			{
				int localValue;

				memmove(&localValue, &m_valueInt, sizeof(int));
				*(double *)value = double(localValue); // enums are 32bit
			}
			else
			{
				*(double *)value = 0;
				return false;
			}
			return true;
		}
		else if (typeId == asTYPEID_INT64)
		{
			if (m_typeId == asTYPEID_DOUBLE)
				*(asINT64 *)value = asINT64(m_valueFlt);
			else if (m_typeId == asTYPEID_BOOL)
			{
				char localValue;

				memmove(&localValue, &m_valueInt, sizeof(char));
				*(asINT64 *)value = localValue ? 1 : 0;
			}
			else if (m_typeId > asTYPEID_DOUBLE && (m_typeId & asTYPEID_MASK_OBJECT) == 0)
			{
				int localValue;

				memmove(&localValue, &m_valueInt, sizeof(int));
				*(asINT64 *)value = localValue; // enums are 32bit
			}
			else
			{
				*(asINT64 *)value = 0;
				return false;
			}
			return true;
		}
		else if (typeId > asTYPEID_DOUBLE && (m_typeId & asTYPEID_MASK_OBJECT) == 0)
		{
			if (m_typeId == asTYPEID_DOUBLE)
				*(int *)value = int(m_valueFlt);
			else if (m_typeId == asTYPEID_INT64)
				*(int *)value = int(m_valueInt);
			else if (m_typeId == asTYPEID_BOOL)
			{
				char localValue;

				memmove(&localValue, &m_valueInt, sizeof(char));
				*(int *)value = localValue ? 1 : 0;
			}
			else if (m_typeId > asTYPEID_DOUBLE && (m_typeId & asTYPEID_MASK_OBJECT) == 0)
			{
				int localValue;

				memmove(&localValue, &m_valueInt, sizeof(int));
				*(int *)value = localValue; // enums are 32bit
			}
			else
			{
				*(int *)value = 0;
				return false;
			}
			return true;
		}
		else if (typeId == asTYPEID_BOOL)
		{
			if (m_typeId & asTYPEID_OBJHANDLE)
			{
				*(bool *)value = m_valueObj ? true : false;
			}
			else if (m_typeId & asTYPEID_MASK_OBJECT)
			{
				*(bool *)value = true;
			}
			else
			{
				asQWORD zero = 0;
				int size = engine->GetSizeOfPrimitiveType(m_typeId);

				*(bool *)value = memcmp(&m_valueInt, &zero, size) == 0 ? false : true;
			}
			return true;
		}
	}

	return false;
}

const void *CScriptDictValue::GetAddressOfValue() const
{
	if ((m_typeId & asTYPEID_MASK_OBJECT) && !(m_typeId & asTYPEID_OBJHANDLE))
	{
		return m_valueObj;
	}

	return reinterpret_cast<const void *>(&m_valueObj);
}

bool CScriptDictValue::Get(asIScriptEngine *engine, asINT64 &value) const
{
	return Get(engine, &value, asTYPEID_INT64);
}

bool CScriptDictValue::Get(asIScriptEngine *engine, double &value) const
{
	return Get(engine, &value, asTYPEID_DOUBLE);
}

int CScriptDictValue::GetTypeId() const
{
	return m_typeId;
}

void CScriptDictValue_Construct(void *mem)
{
	new (mem) CScriptDictValue();
}

void CScriptDictValue_Destruct(CScriptDictValue *obj)
{
	asIScriptContext *ctx = asGetActiveContext();

	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();

		obj->FreeValue(engine);
	}
	obj->~CScriptDictValue();
}

CScriptDictValue &CScriptDictValue_opAssign(void *ref, int typeId, CScriptDictValue *obj)
{
	asIScriptContext *ctx = asGetActiveContext();

	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();

		obj->Set(engine, ref, typeId);
	}
	return *obj;
}

CScriptDictValue &CScriptDictValue_opAssign(const CScriptDictValue &other, CScriptDictValue *obj)
{
	asIScriptContext *ctx = asGetActiveContext();

	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();

		obj->Set(engine, const_cast<CScriptDictValue &>(other));
	}

	return *obj;
}

CScriptDictValue &CScriptDictValue_opAssign(double val, CScriptDictValue *obj)
{
	return CScriptDictValue_opAssign(&val, asTYPEID_DOUBLE, obj);
}

CScriptDictValue &CScriptDictValue_opAssign(asINT64 val, CScriptDictValue *obj)
{
	return CScriptDictValue_opAssign(&val, asTYPEID_INT64, obj);
}

void CScriptDictValue_opCast(void *ref, int typeId, CScriptDictValue *obj)
{
	asIScriptContext *ctx = asGetActiveContext();

	if (ctx)
	{
		asIScriptEngine *engine = ctx->GetEngine();

		obj->Get(engine, ref, typeId);
	}
}

asINT64 CScriptDictValue_opConvInt(CScriptDictValue *obj)
{
	asINT64 value;

	CScriptDictValue_opCast(&value, asTYPEID_INT64, obj);
	return value;
}

double CScriptDictValue_opConvDouble(CScriptDictValue *obj)
{
	double value;

	CScriptDictValue_opCast(&value, asTYPEID_DOUBLE, obj);
	return value;
}

void CScriptDictValue_opConvDouble_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();
	double value;

	self->Get(gen->GetEngine(), value);
	*(double *)gen->GetAddressOfReturnLocation() = value;
}

void CScriptDictValue_opConvInt_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();
	asINT64 value;

	self->Get(gen->GetEngine(), value);
	*(asINT64 *)gen->GetAddressOfReturnLocation() = value;
}

void CScriptDictValue_opCast_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	self->Get(gen->GetEngine(), gen->GetArgAddress(0), gen->GetArgTypeId(0));
}

void CScriptDictValue_opAssign_int64_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	*(CScriptDictValue **)gen->GetAddressOfReturnLocation() = &CScriptDictValue_opAssign((asINT64)gen->GetArgQWord(0), self);
}

void CScriptDictValue_opAssign_double_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	*(CScriptDictValue **)gen->GetAddressOfReturnLocation() = &CScriptDictValue_opAssign(gen->GetArgDouble(0), self);
}

void CScriptDictValue_opAssign_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	*(CScriptDictValue **)gen->GetAddressOfReturnLocation() = &CScriptDictValue_opAssign(gen->GetArgAddress(0), gen->GetArgTypeId(0), self);
}

void CScriptDictValue_opCopyAssign_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	*(CScriptDictValue **)gen->GetAddressOfReturnLocation() =
		&CScriptDictValue_opAssign(*reinterpret_cast<CScriptDictValue *>(gen->GetArgAddress(0)), self);
}

void CScriptDictValue_Construct_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	CScriptDictValue_Construct(self);
}

void CScriptDictValue_Destruct_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	CScriptDictValue_Destruct(self);
}

void CScriptDictValue_EnumReferences_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	self->EnumReferences(gen->GetEngine());
}

void CScriptDictValue_FreeValue_Generic(asIScriptGeneric *gen)
{
	CScriptDictValue *self = (CScriptDictValue *)gen->GetObject();

	self->FreeValue(gen->GetEngine());
}

void RegisterScriptDictionary(asIScriptEngine *engine)
{
	if (strstr(asGetLibraryOptions(), "AS_MAX_PORTABILITY"))
		RegisterScriptDictionary_Generic(engine);
	else
		RegisterScriptDictionary_Native(engine);
}

void RegisterScriptDictionary_Native(asIScriptEngine *engine)
{
#if AS_CAN_USE_CPP11
	engine->RegisterObjectType(
		"dictionaryValue", sizeof(CScriptDictValue), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asGetTypeTraits<CScriptDictValue>());
#else
	engine->RegisterObjectType("dictionaryValue", sizeof(CScriptDictValue), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asOBJ_APP_CLASS_CD);
#endif
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(CScriptDictValue_Construct), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(CScriptDictValue_Destruct), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(CScriptDictValue, EnumReferences), asCALL_THISCALL);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(CScriptDictValue, FreeValue), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opAssign(const dictionaryValue &in)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (const CScriptDictValue &, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opHndlAssign(const ?&in)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (void *, int, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opHndlAssign(const dictionaryValue &in)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (const CScriptDictValue &, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opAssign(const ?&in)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (void *, int, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opAssign(double)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (double, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opAssign(int64)",
		asFUNCTIONPR(CScriptDictValue_opAssign, (asINT64, CScriptDictValue *), CScriptDictValue &), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "void opCast(?&out)",
		asFUNCTIONPR(CScriptDictValue_opCast, (void *, int, CScriptDictValue *), void), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "void opConv(?&out)",
		asFUNCTIONPR(CScriptDictValue_opCast, (void *, int, CScriptDictValue *), void), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod(
		"dictionaryValue", "int64 opConv()", asFUNCTIONPR(CScriptDictValue_opConvInt, (CScriptDictValue *), asINT64), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("dictionaryValue", "double opConv()",
		asFUNCTIONPR(CScriptDictValue_opConvDouble, (CScriptDictValue *), double), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("dictionary", sizeof(CScriptDictionary), asOBJ_REF | asOBJ_GC);

	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_FACTORY, "dictionary@ f()", asFUNCTION(ScriptDictionaryFactory_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_LIST_FACTORY, "dictionary @f(int &in) {repeat {string, ?}}",
		asFUNCTION(ScriptDictionaryListFactory_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ADDREF, "void f()", asMETHOD(CScriptDictionary, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_RELEASE, "void f()", asMETHOD(CScriptDictionary, Release), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "dictionary &opAssign(const dictionary &in)",
		asMETHODPR(CScriptDictionary, operator=, (const CScriptDictionary &), CScriptDictionary &), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "void set(const string &in, const ?&in)",
		asMETHODPR(CScriptDictionary, Set, (const dictKey_t &, void *, int), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "bool get(const string &in, ?&out) const",
		asMETHODPR(CScriptDictionary, Get, (const dictKey_t &, void *, int) const, bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "void set(const string &in, const int64&in)",
		asMETHODPR(CScriptDictionary, Set, (const dictKey_t &, const asINT64 &), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "bool get(const string &in, int64&out) const",
		asMETHODPR(CScriptDictionary, Get, (const dictKey_t &, asINT64 &) const, bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "void set(const string &in, const double&in)",
		asMETHODPR(CScriptDictionary, Set, (const dictKey_t &, const double &), void), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "bool get(const string &in, double&out) const",
		asMETHODPR(CScriptDictionary, Get, (const dictKey_t &, double &) const, bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "bool exists(const string &in) const", asMETHOD(CScriptDictionary, Exists), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "bool isEmpty() const", asMETHOD(CScriptDictionary, IsEmpty), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "uint getSize() const", asMETHOD(CScriptDictionary, GetSize), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "bool delete(const string &in)", asMETHOD(CScriptDictionary, Delete), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "void deleteAll()", asMETHOD(CScriptDictionary, DeleteAll), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "array<string> @getKeys() const", asMETHOD(CScriptDictionary, GetKeys), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "dictionaryValue &opIndex(const string &in)",
		asMETHODPR(CScriptDictionary, operator[], (const dictKey_t &), CScriptDictValue *), asCALL_THISCALL);
	engine->RegisterObjectMethod("dictionary", "const dictionaryValue &opIndex(const string &in) const",
		asMETHODPR(CScriptDictionary, operator[], (const dictKey_t &) const, const CScriptDictValue *), asCALL_THISCALL);

	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_GETREFCOUNT, "int f()", asMETHOD(CScriptDictionary, GetRefCount), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_SETGCFLAG, "void f()", asMETHOD(CScriptDictionary, SetGCFlag), asCALL_THISCALL);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_GETGCFLAG, "bool f()", asMETHOD(CScriptDictionary, GetGCFlag), asCALL_THISCALL);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_ENUMREFS, "void f(int&in)", asMETHOD(CScriptDictionary, EnumReferences), asCALL_THISCALL);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_RELEASEREFS, "void f(int&in)", asMETHOD(CScriptDictionary, ReleaseAllReferences), asCALL_THISCALL);

#if AS_USE_STLNAMES == 1
	engine->RegisterObjectMethod("dictionary", "bool empty() const", asMETHOD(CScriptDictionary, IsEmpty), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "uint size() const", asMETHOD(CScriptDictionary, GetSize), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "void erase(const string &in)", asMETHOD(CScriptDictionary, Delete), asCALL_THISCALL);

	engine->RegisterObjectMethod("dictionary", "void clear()", asMETHOD(CScriptDictionary, DeleteAll), asCALL_THISCALL);
#endif

	SDictionaryCache::Setup(engine);
}

void RegisterScriptDictionary_Generic(asIScriptEngine *engine)
{
	engine->SetEngineUserDataCleanupCallback(SDictionaryCache::Cleanup, DICTIONARY_CACHE);

#if AS_CAN_USE_CPP11
	engine->RegisterObjectType(
		"dictionaryValue", sizeof(CScriptDictValue), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asGetTypeTraits<CScriptDictValue>());
#else
	engine->RegisterObjectType("dictionaryValue", sizeof(CScriptDictValue), asOBJ_VALUE | asOBJ_ASHANDLE | asOBJ_GC | asOBJ_APP_CLASS_CD);
#endif
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(CScriptDictValue_Construct_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(CScriptDictValue_Destruct_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_ENUMREFS, "void f(int&in)", asFUNCTION(CScriptDictValue_EnumReferences_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionaryValue", asBEHAVE_RELEASEREFS, "void f(int&in)", asFUNCTION(CScriptDictValue_FreeValue_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opAssign(const dictionaryValue &in)",
		asFUNCTION(CScriptDictValue_opCopyAssign_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionaryValue", "dictionaryValue &opHndlAssign(const ?&in)", asFUNCTION(CScriptDictValue_opAssign_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "dictionaryValue &opHndlAssign(const dictionaryValue &in)",
		asFUNCTION(CScriptDictValue_opCopyAssign_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionaryValue", "dictionaryValue &opAssign(const ?&in)", asFUNCTION(CScriptDictValue_opAssign_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionaryValue", "dictionaryValue &opAssign(double)", asFUNCTION(CScriptDictValue_opAssign_double_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionaryValue", "dictionaryValue &opAssign(int64)", asFUNCTION(CScriptDictValue_opAssign_int64_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "void opCast(?&out)", asFUNCTION(CScriptDictValue_opCast_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "void opConv(?&out)", asFUNCTION(CScriptDictValue_opCast_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "int64 opConv()", asFUNCTION(CScriptDictValue_opConvInt_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionaryValue", "double opConv()", asFUNCTION(CScriptDictValue_opConvDouble_Generic), asCALL_GENERIC);

	engine->RegisterObjectType("dictionary", sizeof(CScriptDictionary), asOBJ_REF | asOBJ_GC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_FACTORY, "dictionary@ f()", asFUNCTION(ScriptDictionaryFactory_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_LIST_FACTORY, "dictionary @f(int &in) {repeat {string, ?}}",
		asFUNCTION(ScriptDictionaryListFactory_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour("dictionary", asBEHAVE_ADDREF, "void f()", asFUNCTION(ScriptDictionaryAddRef_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_RELEASE, "void f()", asFUNCTION(ScriptDictionaryRelease_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "dictionary &opAssign(const dictionary &in)", asFUNCTION(ScriptDictionaryAssign_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "void set(const string &in, const ?&in)", asFUNCTION(ScriptDictionarySet_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionary", "bool get(const string &in, ?&out) const", asFUNCTION(ScriptDictionaryGet_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "void set(const string &in, const int64&in)", asFUNCTION(ScriptDictionarySetInt_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionary", "bool get(const string &in, int64&out) const", asFUNCTION(ScriptDictionaryGetInt_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "void set(const string &in, const double&in)", asFUNCTION(ScriptDictionarySetFlt_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod(
		"dictionary", "bool get(const string &in, double&out) const", asFUNCTION(ScriptDictionaryGetFlt_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "bool exists(const string &in) const", asFUNCTION(ScriptDictionaryExists_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionary", "bool isEmpty() const", asFUNCTION(ScriptDictionaryIsEmpty_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionary", "uint getSize() const", asFUNCTION(ScriptDictionaryGetSize_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionary", "bool delete(const string &in)", asFUNCTION(ScriptDictionaryDelete_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionary", "void deleteAll()", asFUNCTION(ScriptDictionaryDeleteAll_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "array<string> @getKeys() const", asFUNCTION(CScriptDictionaryGetKeys_Generic), asCALL_GENERIC);

	engine->RegisterObjectMethod(
		"dictionary", "dictionaryValue &opIndex(const string &in)", asFUNCTION(CScriptDictionary_opIndex_Generic), asCALL_GENERIC);
	engine->RegisterObjectMethod("dictionary", "const dictionaryValue &opIndex(const string &in) const",
		asFUNCTION(CScriptDictionary_opIndex_const_Generic), asCALL_GENERIC);

	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_GETREFCOUNT, "int f()", asFUNCTION(ScriptDictionaryGetRefCount_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_SETGCFLAG, "void f()", asFUNCTION(ScriptDictionarySetGCFlag_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_GETGCFLAG, "bool f()", asFUNCTION(ScriptDictionaryGetGCFlag_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_ENUMREFS, "void f(int&in)", asFUNCTION(ScriptDictionaryEnumReferences_Generic), asCALL_GENERIC);
	engine->RegisterObjectBehaviour(
		"dictionary", asBEHAVE_RELEASEREFS, "void f(int&in)", asFUNCTION(ScriptDictionaryReleaseAllReferences_Generic), asCALL_GENERIC);

	SDictionaryCache::Setup(engine);
}

CScriptDictionary::CIterator CScriptDictionary::begin() const
{
	return CIterator(*this, dict.begin());
}

CScriptDictionary::CIterator CScriptDictionary::end() const
{
	return CIterator(*this, dict.end());
}

CScriptDictionary::CIterator CScriptDictionary::find(const dictKey_t &key) const
{
	return CIterator(*this, dict.find(key));
}

CScriptDictionary::CIterator::CIterator(const CScriptDictionary &dict, dictMap_t::const_iterator it)
	: m_it(it), m_dict(dict)
{
}

void CScriptDictionary::CIterator::operator++()
{
	m_it++;
}

void CScriptDictionary::CIterator::operator++(int)
{
	m_it++;
}

CScriptDictionary::CIterator &CScriptDictionary::CIterator::operator*()
{
	return *this;
}

bool CScriptDictionary::CIterator::operator==(const CIterator &other) const
{
	return m_it == other.m_it;
}

bool CScriptDictionary::CIterator::operator!=(const CIterator &other) const
{
	return m_it != other.m_it;
}

const dictKey_t &CScriptDictionary::CIterator::GetKey() const
{
	return m_it->first;
}

int CScriptDictionary::CIterator::GetTypeId() const
{
	return m_it->second.m_typeId;
}

bool CScriptDictionary::CIterator::GetValue(asINT64 &value) const
{
	return m_it->second.Get(m_dict.engine, &value, asTYPEID_INT64);
}

bool CScriptDictionary::CIterator::GetValue(double &value) const
{
	return m_it->second.Get(m_dict.engine, &value, asTYPEID_DOUBLE);
}

bool CScriptDictionary::CIterator::GetValue(void *value, int typeId) const
{
	return m_it->second.Get(m_dict.engine, value, typeId);
}

const void *CScriptDictionary::CIterator::GetAddressOfValue() const
{
	return m_it->second.GetAddressOfValue();
}

END_AS_NAMESPACE
