#ifndef CCUSTOMKEYVALUES_H
#define CCUSTOMKEYVALUES_H

#include "server/entvars_types.h"

#include <vector>

enum Ownership
{
	OWNERSHIP_NO_DELETE = 0,
	OWNERSHIP_DELETE,
	OWNERSHIP_BY_TYPE
};

enum CCustomKeyvaluesNote
{
	CUSTOM_KEYVALUES_INVALID_TYPE = 0,
	CUSTOM_KEYVALUES_INVALID_SPECIFIER,
	CUSTOM_KEYVALUES_DUPLICATE_INSERT,
	CUSTOM_KEYVALUES_MAX_HEIGHT
};

enum { DEFAULT_CUSTOM_KEYVALUES_MAX_HEIGHT = 64, MAX_CUSTOM_KEYVALUE_PRINT_TEXT = 1024 };

struct DynKeyvalue
{
	char			*m_pszName;
	void			*m_pValue;
	entvartype_s	m_Type;
};

struct DynKVNode
{
	DynKeyvalue	*m_pKeyvalue;
	bool		m_bOwnsValue;
	DynKVNode	*m_pLeft;
	DynKVNode	*m_pRight;
	int			m_iDepth;
};

class CCustomKeyvalues
{
public:
	explicit CCustomKeyvalues(int maxHeight);
	~CCustomKeyvalues();

	void Init(void);
	static entvartype_s InterpretSpecifier(const char *name);
	static bool IsCustomKeyvalue(const char *name);
	bool RemoveKeyvalue(const char *name);
	DynKVNode *Find(const char *name);
	void *GetKeyvalue(const char *name, entvartype_s &type);
	bool HasKeyvalue(const char *name);
	DynKVNode **GiveHeadLink(int depth);
	void DestroyNode(DynKVNode *node);
	void DestroyTree(DynKVNode *node);
	int CalcDepth(DynKVNode *node);
	int GiveDepth(DynKVNode *node);
	int MakeDepth(DynKVNode *node);
	DynKVNode *RotateLeft(DynKVNode *node);
	DynKVNode *RotateRight(DynKVNode *node);
	void Remove(int depth);
	int CalcBalance(DynKVNode *node);
	void RebalanceFromDepth(int depth);
	void Note(CCustomKeyvaluesNote note, ...);
	int FindInsertWithTrace(const char *name);
	void Insert(DynKVNode *node);
	int FindRemoveWithTrace(const char *name);
	bool SetKeyvalue(const char *name, const void *value, entvartype_s type, Ownership ownership);
	static const char *DefaultForType(entvartype_s type);
	void *InitializeKeyvalueWithDefault(const char *name, entvartype_s &type);
	int PrintKeyvalues(DynKVNode *node);
	void PrintKeyvalues(void);

private:
	DynKVNode	*m_pRoot;
	std::vector<DynKVNode *>	m_ppTrace;
	std::vector<bool>		m_pbTraceDirections;
	bool		m_bInitialized;
	int			m_iCount;
	int			m_iMaxHeight;

	static const char *nullString;
	static const char *nullFloat;
	static const char *nullInt;
	static const char *nullVec;
};

#endif
