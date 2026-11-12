#ifndef TYPEDENTITYHANDLE_H
#define TYPEDENTITYHANDLE_H

template<class T>
class CEntityHandle
{
public:
	edict_t *Get( void ) const
	{
		return const_cast<EHANDLE &>( m_Handle ).Get();
	}

	edict_t *Set( edict_t *entity )
	{
		return m_Handle.Set( entity );
	}

	operator T *() const
	{
		edict_t *entity = Get();

		if ( !entity || entity->free )
			return NULL;

		return static_cast<T *>( (CBaseEntity *) entity->pvPrivateData );
	}

	T *operator ->() const
	{
		return (T *) *this;
	}

	T *operator =( T *entity )
	{
		m_Handle = (CBaseEntity *) NULL;

		if ( !entity || !entity->pev )
			return NULL;

		edict_t *edict = entity->pev->pContainingEntity;

		if ( !edict || edict->free )
			return NULL;

		m_Handle.Set( edict );
		return entity;
	}

	EHANDLE m_Handle;
};

#endif
