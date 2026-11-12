#ifndef ENTITYHANDLE_H
#define ENTITYHANDLE_H

#include "extdll.h"
#include <functional>

template<class T>
class EntityHandle
{
public:
	EntityHandle() : m_pEdict(NULL), m_iSerialNumber(0) {}

	EntityHandle(const EntityHandle &other)
		: m_pEdict(other.m_pEdict), m_iSerialNumber(other.m_iSerialNumber)
	{
	}

	EntityHandle(T *entity) : m_pEdict(NULL), m_iSerialNumber(0)
	{
		*this = entity;
	}

	~EntityHandle() {}

	EntityHandle &operator=(const EntityHandle &other)
	{
		m_pEdict = other.m_pEdict;
		m_iSerialNumber = other.m_iSerialNumber;
		return *this;
	}

	T *operator=(T *entity)
	{
		m_pEdict = NULL;
		m_iSerialNumber = 0;

		if (!entity || !entity->pev)
			return NULL;

		edict_t *edict = entity->pev->pContainingEntity;

		if (!edict || edict->free)
			return NULL;

		m_pEdict = edict;
		m_iSerialNumber = edict->serialnumber;
		return *this;
	}

	operator T *()
	{
		if (!m_pEdict || m_pEdict->serialnumber != m_iSerialNumber || m_pEdict->free)
			return NULL;

		return (T *)m_pEdict->pvPrivateData;
	}

	int GetSerialNumber() const
	{
		return m_iSerialNumber;
	}

	struct Hash
	{
		size_t operator()(const EntityHandle &handle) const
		{
			return std::hash<edict_t *>()(handle.m_pEdict) ^ std::hash<int>()(handle.m_iSerialNumber);
		}
	};

	struct Equal
	{
		bool operator()(const EntityHandle &left, const EntityHandle &right) const
		{
			return left.m_iSerialNumber == right.m_iSerialNumber && left.m_pEdict == right.m_pEdict;
		}
	};

private:
	edict_t	*m_pEdict;
	int		m_iSerialNumber;
};

#endif
