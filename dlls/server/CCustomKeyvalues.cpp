#include "extdll.h"
#include "util.h"
#include "server/server_support.h"
#include "server/CCustomKeyvalues.h"
#include <stdarg.h>

const char *CCustomKeyvalues::nullString = "";
const char *CCustomKeyvalues::nullFloat = "0.0";
const char *CCustomKeyvalues::nullInt = "0";
const char *CCustomKeyvalues::nullVec = "0.0 0.0 0.0";

CCustomKeyvalues::CCustomKeyvalues(int maxHeight)
{
	m_pRoot = NULL;
	m_bInitialized = false;
	m_iCount = 0;
	m_iMaxHeight = maxHeight;
}

CCustomKeyvalues::~CCustomKeyvalues()
{
	if (m_bInitialized)
	{
		DestroyTree(m_pRoot);
		delete m_ppTrace[0];
	}
}

void CCustomKeyvalues::Init(void)
{
	if (m_bInitialized)
		return;

	m_ppTrace.assign(m_iMaxHeight + 1, NULL);
	m_ppTrace[0] = new DynKVNode();
	m_pbTraceDirections.assign(m_iMaxHeight + 1, false);
	m_bInitialized = true;
}

entvartype_s CCustomKeyvalues::InterpretSpecifier(const char *name)
{
	if (name[0] != '$')
		return VAR_INVALID;

	entvartype_s type;

	switch (name[1])
	{
	case 's':
		type = VAR_STRING;
		break;
	case 'v':
		type = VAR_VECTOR;
		break;
	case 'f':
		type = VAR_FLOAT;
		break;
	case 'i':
		type = VAR_INTEGER;
		break;
	default:
		return VAR_INVALID;
	}

	return name[2] == '_' ? type : VAR_INVALID;
}

bool CCustomKeyvalues::IsCustomKeyvalue(const char *name)
{
	return name[0] == '$';
}

bool CCustomKeyvalues::RemoveKeyvalue(const char *name)
{
	ALERT(at_error, "Error: Call to unimplemented function: CCustomKeyvalues::RemoveKeyvalue(\"%s\").\n", name);
	return false;
}

DynKVNode *CCustomKeyvalues::Find(const char *name)
{
	DynKVNode *node = m_pRoot;

	while (node)
	{
		int result = strcmp(name, node->m_pKeyvalue->m_pszName);

		if (!result)
			return node;

		node = result < 0 ? node->m_pLeft : node->m_pRight;
	}

	return NULL;
}

void *CCustomKeyvalues::GetKeyvalue(const char *name, entvartype_s &type)
{
	DynKVNode *node = Find(name);

	if (!node)
	{
		type = VAR_INVALID;
		return NULL;
	}

	type = node->m_pKeyvalue->m_Type;
	return node->m_pKeyvalue->m_pValue;
}

bool CCustomKeyvalues::HasKeyvalue(const char *name)
{
	return Find(name) != NULL;
}

DynKVNode **CCustomKeyvalues::GiveHeadLink(int depth)
{
	DynKVNode *head = m_ppTrace[depth - 1];

	return m_pbTraceDirections[depth - 1] ? &head->m_pRight : &head->m_pLeft;
}

void CCustomKeyvalues::DestroyNode(DynKVNode *node)
{
	if (!node)
		return;

	free(node->m_pKeyvalue->m_pszName);

	if (node->m_bOwnsValue)
		::operator delete(node->m_pKeyvalue->m_pValue);

	delete node->m_pKeyvalue;
	delete node;
	m_iCount--;
}

void CCustomKeyvalues::DestroyTree(DynKVNode *node)
{
	if (!node)
		return;

	DestroyTree(node->m_pLeft);
	DestroyTree(node->m_pRight);
	DestroyNode(node);
}

int CCustomKeyvalues::CalcDepth(DynKVNode *node)
{
	if (!node)
		return 0;

	return V_max(CalcDepth(node->m_pLeft), CalcDepth(node->m_pRight)) + 1;
}

int CCustomKeyvalues::GiveDepth(DynKVNode *node)
{
	return node ? node->m_iDepth : 0;
}

int CCustomKeyvalues::MakeDepth(DynKVNode *node)
{
	if (!node)
		return 0;

	return V_max(GiveDepth(node->m_pLeft), GiveDepth(node->m_pRight)) + 1;
}

DynKVNode *CCustomKeyvalues::RotateLeft(DynKVNode *node)
{
	if (!node || !node->m_pRight)
		return node;

	DynKVNode *right = node->m_pRight;

	node->m_pRight = right->m_pLeft;
	right->m_pLeft = node;
	node->m_iDepth = MakeDepth(node);
	right->m_iDepth = MakeDepth(right);
	return right;
}

DynKVNode *CCustomKeyvalues::RotateRight(DynKVNode *node)
{
	if (!node || !node->m_pLeft)
		return node;

	DynKVNode *left = node->m_pLeft;

	node->m_pLeft = left->m_pRight;
	left->m_pRight = node;
	node->m_iDepth = MakeDepth(node);
	left->m_iDepth = MakeDepth(left);
	return left;
}

void CCustomKeyvalues::Remove(int depth)
{
	DynKVNode *node = m_ppTrace[depth];

	if (!node)
		return;

	DynKVNode **head = GiveHeadLink(depth);
	DynKVNode *left = node->m_pLeft;
	DynKVNode *right = node->m_pRight;

	if (left)
	{
		if (right)
		{
			if (left->m_iDepth >= right->m_iDepth)
			{
				if (left->m_pRight)
				{
					while (right->m_pLeft)
					{
						right->m_iDepth = V_max(GiveDepth(right->m_pRight),
							GiveDepth(right->m_pLeft) + left->m_pRight->m_iDepth) + 1;
						right = right->m_pLeft;
					}

					right->m_pLeft = left->m_pRight;
					right->m_iDepth = MakeDepth(right);
				}

				left->m_pRight = node->m_pRight;
				left->m_iDepth = MakeDepth(left);
				*head = left;
			}
			else
			{
				if (right->m_pLeft)
				{
					while (left->m_pRight)
					{
						left->m_iDepth = V_max(GiveDepth(left->m_pLeft),
							GiveDepth(left->m_pRight) + right->m_pLeft->m_iDepth) + 1;
						left = left->m_pRight;
					}

					left->m_pRight = right->m_pLeft;
					left->m_iDepth = MakeDepth(left);
				}

				right->m_pLeft = node->m_pLeft;
				right->m_iDepth = MakeDepth(right);
				*head = right;
			}
		}
		else
		{
			*head = left;
		}
	}
	else
	{
		*head = right;
	}

	DestroyNode(node);
	m_ppTrace[depth - 1]->m_iDepth = MakeDepth(m_ppTrace[depth - 1]);
	m_pRoot = m_ppTrace[0]->m_pLeft;
}

int CCustomKeyvalues::CalcBalance(DynKVNode *node)
{
	if (!node)
		return 0;

	return GiveDepth(node->m_pRight) - GiveDepth(node->m_pLeft);
}

void CCustomKeyvalues::RebalanceFromDepth(int depth)
{
	for (; depth >= 1; depth--)
	{
		DynKVNode *node = m_ppTrace[depth];

		node->m_iDepth = MakeDepth(node);

		int balance = CalcBalance(node);

		if (balance > 1)
		{
			DynKVNode **head = GiveHeadLink(depth);

			if (!m_pbTraceDirections[depth + 1])
			{
				node->m_pRight = RotateRight(m_ppTrace[depth + 1]);
				node->m_iDepth = MakeDepth(node);
			}

			*head = RotateLeft(node);
		}
		else if (balance < -1)
		{
			DynKVNode **head = GiveHeadLink(depth);

			if (m_pbTraceDirections[depth + 1])
			{
				node->m_pLeft = RotateLeft(m_ppTrace[depth + 1]);
				node->m_iDepth = MakeDepth(node);
			}

			*head = RotateRight(node);
		}
	}

	m_pRoot = m_ppTrace[0]->m_pLeft;
}

void CCustomKeyvalues::Note(CCustomKeyvaluesNote note, ...)
{
	switch (note)
	{
	case CUSTOM_KEYVALUES_INVALID_TYPE:
		ALERT(at_console, "Fatal internal error in CCustomKeyvalues when determining custom keyvalue type.\n");
		break;
	case CUSTOM_KEYVALUES_INVALID_SPECIFIER:
	{
		va_list args;

		va_start(args, note);
		const char *name = va_arg(args, const char *);
		va_end(args);
		ALERT(at_console, "Error in CCustomKeyvalues: Unknown or no type-specifier in key-name \"%s\".\n", name);
		break;
	}
	case CUSTOM_KEYVALUES_DUPLICATE_INSERT:
		ALERT(at_console, "Fatal internal error in CCustomKeyvalues: Tried keyvalue insertion when it already existed.\n");
		break;
	case CUSTOM_KEYVALUES_MAX_HEIGHT:
		ALERT(at_console, "Fatal internal error in CCustomKeyvalues: Reached maximum AVL-Tree-height (%i).\n", m_iMaxHeight);
		break;
	default:
		ALERT(at_console, "Unknown error message type in CCustomKeyvalues.\n");
		break;
	}
}

int CCustomKeyvalues::FindInsertWithTrace(const char *name)
{
	DynKVNode *node = m_pRoot;
	int depth = 1;

	while (node)
	{
		m_ppTrace[depth] = node;

		int result = strcmp(name, node->m_pKeyvalue->m_pszName);

		if (!result)
		{
			Note(CUSTOM_KEYVALUES_DUPLICATE_INSERT);
			return -1;
		}

		m_pbTraceDirections[depth] = result > 0;
		node = result < 0 ? node->m_pLeft : node->m_pRight;

		depth++;

		if (depth > m_iMaxHeight)
		{
			Note(CUSTOM_KEYVALUES_MAX_HEIGHT);
			return -1;
		}
	}

	return depth - 1;
}

void CCustomKeyvalues::Insert(DynKVNode *node)
{
	int depth = FindInsertWithTrace(node->m_pKeyvalue->m_pszName);

	if (depth == -1)
		return;

	DynKVNode *head = m_ppTrace[depth];

	if (m_pbTraceDirections[depth])
		head->m_pRight = node;
	else
		head->m_pLeft = node;

	m_iCount++;
	RebalanceFromDepth(depth);
}

int CCustomKeyvalues::FindRemoveWithTrace(const char *name)
{
	DynKVNode *node = m_pRoot;
	int depth = 1;

	while (node)
	{
		m_ppTrace[depth] = node;

		int result = strcmp(name, node->m_pKeyvalue->m_pszName);

		if (!result)
		{
			if (depth < m_iMaxHeight)
				return depth;

			break;
		}

		m_pbTraceDirections[depth] = result > 0;
		node = result < 0 ? node->m_pLeft : node->m_pRight;

		if (!node)
			return -1;

		depth++;

		if (depth > m_iMaxHeight)
			break;
	}

	if (node)
		Note(CUSTOM_KEYVALUES_MAX_HEIGHT);

	return -1;
}

bool CCustomKeyvalues::SetKeyvalue(const char *name, const void *value, entvartype_s type, Ownership ownership)
{
	Init();

	DynKVNode *node = Find(name);
	entvartype_s valueType = type;

	if (type == VAR_CUSTOM)
	{
		if (IsCustomKeyvalue(name))
		{
			valueType = node ? node->m_pKeyvalue->m_Type : InterpretSpecifier(name);

			if (!node && valueType == VAR_INVALID)
			{
				Note(CUSTOM_KEYVALUES_INVALID_SPECIFIER, name);
				return false;
			}
		}
		else
		{
			valueType = UTIL_FigureVarType((const char *)value);
		}
	}

	bool replaceValue = !node || valueType != node->m_pKeyvalue->m_Type;

	if (node && replaceValue && node->m_bOwnsValue)
		::operator delete(node->m_pKeyvalue->m_pValue);

	void *storedValue = node ? node->m_pKeyvalue->m_pValue : NULL;

	switch (valueType)
	{
	case VAR_STRING:
		if (replaceValue)
			storedValue = new string_t;

		*(string_t *)storedValue = type == VAR_CUSTOM ?
			AllocStringLocal((const char *)value) : *(const string_t *)value;
		break;

	case VAR_VECTOR:
		if (replaceValue)
			storedValue = new Vector;

		if (type == VAR_CUSTOM)
			UTIL_StringToVector((float *)storedValue, (const char *)value, ' ');
		else
			*(Vector *)storedValue = *(const Vector *)value;
		break;

	case VAR_FLOAT:
		if (replaceValue)
			storedValue = new float;

		*(float *)storedValue = type == VAR_CUSTOM ? (float)atof((const char *)value) : *(const float *)value;
		break;

	case VAR_INTEGER:
		if (replaceValue)
			storedValue = new int;

		*(int *)storedValue = type == VAR_CUSTOM ? atoi((const char *)value) : *(const int *)value;
		break;

	default:
		Note(CUSTOM_KEYVALUES_INVALID_TYPE);
		return false;
	}

	bool ownsValue = ownership == OWNERSHIP_BY_TYPE ? valueType != VAR_STRING : ownership == OWNERSHIP_DELETE;

	if (node)
	{
		// Overwrites retain the previous entry's type tag
		node->m_pKeyvalue->m_pValue = storedValue;
		node->m_bOwnsValue = ownsValue;
		return true;
	}

	DynKeyvalue *keyvalue = new DynKeyvalue();

#ifdef _MSC_VER
	keyvalue->m_pszName = _strdup(name);
#else
	keyvalue->m_pszName = strdup(name);
#endif
	keyvalue->m_pValue = storedValue;
	keyvalue->m_Type = valueType;
	node = new DynKVNode();
	node->m_pKeyvalue = keyvalue;
	node->m_bOwnsValue = ownsValue;
	node->m_iDepth = 1;
	Insert(node);
	return false;
}

const char *CCustomKeyvalues::DefaultForType(entvartype_s type)
{
	switch (type)
	{
	case VAR_STRING:
		return nullString;
	case VAR_VECTOR:
		return nullVec;
	case VAR_FLOAT:
		return nullFloat;
	default:
		return nullInt;
	}
}

void *CCustomKeyvalues::InitializeKeyvalueWithDefault(const char *name, entvartype_s &type)
{
	SetKeyvalue(name, DefaultForType(VAR_STRING), VAR_CUSTOM, OWNERSHIP_BY_TYPE);
	return GetKeyvalue(name, type);
}

int CCustomKeyvalues::PrintKeyvalues(DynKVNode *node)
{
	if (!node)
		return 0;

	int count = PrintKeyvalues(node->m_pLeft);
	DynKeyvalue *keyvalue = node->m_pKeyvalue;
	char text[MAX_CUSTOM_KEYVALUE_PRINT_TEXT] = {};

	switch (keyvalue->m_Type)
	{
	case VAR_STRING:
		snprintf(text, sizeof(text), "%s", STRING(*(string_t *)keyvalue->m_pValue));
		break;
	case VAR_VECTOR:
	{
		Vector &vector = *(Vector *)keyvalue->m_pValue;

		snprintf(text, sizeof(text), "%f %f %f", vector.x, vector.y, vector.z);
		break;
	}
	case VAR_FLOAT:
		snprintf(text, sizeof(text), "%f", *(float *)keyvalue->m_pValue);
		break;
	case VAR_INTEGER:
		snprintf(text, sizeof(text), "%i", *(int *)keyvalue->m_pValue);
		break;
	default:
		break;
	}

	m_iCount++;
	ALERT(at_console, "    #%3i: Key \"%s\" with %s \"%s\"\n",
		m_iCount, keyvalue->m_pszName, EntVarStr(keyvalue->m_Type), text);
	return count + PrintKeyvalues(node->m_pRight) + 1;
}

void CCustomKeyvalues::PrintKeyvalues(void)
{
	m_iCount = 0;
	ALERT(at_console, "{\n");
	PrintKeyvalues(m_pRoot);
	ALERT(at_console, "} // Total tree-height is %i with %i entries.\n", GiveDepth(m_pRoot), m_iCount);
}
