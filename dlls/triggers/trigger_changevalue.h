#ifndef TRIGGER_CHANGEVALUE_H
#define TRIGGER_CHANGEVALUE_H

#define SF_VALUE_INVERT_TARGET 32
#define SF_VALUE_INVERT_SOURCE 64

#define VALUE_MAX_STRING 1023

enum ValueOperation
{
	VALUE_OP_REPLACE = 0,
	VALUE_OP_ADD,
	VALUE_OP_MULTIPLY,
	VALUE_OP_SUBTRACT,
	VALUE_OP_DIVIDE,
	VALUE_OP_AND,
	VALUE_OP_OR,
	VALUE_OP_NAND,
	VALUE_OP_NOR,
	VALUE_OP_DIR_TO_ANGLES,
	VALUE_OP_ANGLES_TO_DIR,
	VALUE_OP_CONCAT,
	VALUE_OP_MODULO,
	VALUE_OP_XOR,
	VALUE_OP_XNOR,
	VALUE_OP_FORMAT,
	VALUE_OP_POWER,
	VALUE_OP_SINE,
	VALUE_OP_COSINE,
	VALUE_OP_TANGENT,
	VALUE_OP_ARCSINE,
	VALUE_OP_ARCCOSINE,
	VALUE_OP_ARCTANGENT,
	VALUE_OP_COTANGENT,
	VALUE_OP_ARCCOTANGENT
};

class CTriggerChangeValue : public CBaseDelay
{
public:
	CTriggerChangeValue();

	void Spawn( void ) override;
	void KeyValue( KeyValueData *pkvd ) override;
	void *GetKeyValue( const char *name, entvartype_s &type ) override;
	void Use( CBaseEntity *activator, CBaseEntity *caller, USE_TYPE useType, float value ) override;
	int ObjectCaps( void ) override { return 0; }
	int Save( CSave &save ) override;
	int Restore( CRestore &restore ) override;

	static TYPEDESCRIPTION m_SaveData[];

private:
	friend class CTriggerCreateEntity;

	float MakeVecLength( Vector *value );
	bool TryKeyValue( CBaseEntity *entity, const char *name, const char *value );
	void SpecialValueFloat( float *value, float source );
	void SpecialValueVector( Vector *value, Vector source );
	void SpecialValueInt( int *value, int source );
	void SpecialValueByteArray( int *value, const char *source, CBaseEntity *destination );
	bool SpecialValue( CBaseEntity *entity, const char *name, const char *value );
	void ChangeValue( CBaseEntity *entity );

	EHANDLE m_hActivator;
	int     m_iAppendSpaces;
	int     m_trigonometricBehaviour;
	int     m_iVectorContext; // Suppresses per-component diagnostics

	string_t m_iszValueName;
	string_t m_iszNewValue;
	int      m_iszValueType;
};

#endif
