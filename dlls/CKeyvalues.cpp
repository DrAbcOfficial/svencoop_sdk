#include "extdll.h"
#include "enginecallback.h"
#include "CKeyvalues.h"

enum { KEYVALUE_INDENT_WIDTH = 4 };

CKeyvalueNode::CKeyvalueNode(KeyvalueNodeType type)
	: m_Type(type)
{
}

CKeyvalueNode::CKeyvalueNode(const char *key, KeyvalueNodeType type)
	: m_Type(type)
{
	SetKey(key);
}

CKeyvalueNode::~CKeyvalueNode()
{
}

void CKeyvalueNode::SetKey(const char *key)
{
	m_Key = key && *key ? key : "INVALID_KEY";
}

void CKeyvalueNode::SetKey(const CString &key)
{
	SetKey(key.Data());
}

CKeyvalue::CKeyvalue()
	: CKeyvalueNode(KEYVALUE_NODE_VALUE)
{
}

CKeyvalue::CKeyvalue(const char *key, const char *value)
	: CKeyvalueNode(key, KEYVALUE_NODE_VALUE)
{
	SetValue(value);
}

CKeyvalue::~CKeyvalue()
{
}

void CKeyvalue::Print(unsigned int level) const
{
	ALERT(at_console, "%*s\"%s\" \"%s\"\n",
		KEYVALUE_INDENT_WIDTH * level, "", GetKey().Data(), m_Value.Data());
}

void CKeyvalue::SetValue(const char *value)
{
	m_Value = value;
}

void CKeyvalue::SetValue(const CString &value)
{
	SetValue(value.Data());
}

CKvBlockNode::CKvBlockNode(const char *key)
	: CKeyvalueNode(key, KEYVALUE_NODE_BLOCK)
{
}

CKvBlockNode::CKvBlockNode(const char *key, const Children &children)
	: CKeyvalueNode(key, KEYVALUE_NODE_BLOCK)
{
	SetChildren(children);
}

CKvBlockNode::CKvBlockNode(const char *key, std::shared_ptr<CKeyvalueNode> child)
	: CKeyvalueNode(key, KEYVALUE_NODE_BLOCK)
{
	m_Children.push_back(child);
}

CKvBlockNode::~CKvBlockNode()
{
}

void CKvBlockNode::Print(unsigned int level) const
{
	ALERT(at_console, "%*s\"%s\"\n%*s{\n",
		KEYVALUE_INDENT_WIDTH * level, "", GetKey().Data(),
		KEYVALUE_INDENT_WIDTH * level, "");
	PrintChildren(level + 1);
	ALERT(at_console, "%*s}\n", KEYVALUE_INDENT_WIDTH * level, "");
}

void CKvBlockNode::PrintChildren(unsigned int level) const
{
	for (Children::const_iterator it = m_Children.begin(); it != m_Children.end(); it++)
		(*it)->Print(level);
}

void CKvBlockNode::SetChildren(const Children &children)
{
	m_Children = children;
}

void CKvBlockNode::RemoveAllChildren(void)
{
	m_Children.clear();
}

void CKvBlockNode::RemoveAllNotNamed(const char *key)
{
	if (!key || !*key)
		return;

	Children::iterator it = m_Children.begin();

	while (it != m_Children.end())
	{
		if (strcmp((*it)->GetKey().Data(), key))
			it = m_Children.erase(it);
		else
			it++;
	}
}

std::shared_ptr<CKeyvalueNode> CKvBlockNode::FindFirstChild(const char *key) const
{
	if (!key || !*key)
		return std::shared_ptr<CKeyvalueNode>();

	for (Children::const_iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		if (!strcmp(key, (*it)->GetKey().Data()))
			return *it;
	}

	return std::shared_ptr<CKeyvalueNode>();
}

std::shared_ptr<CKeyvalueNode> CKvBlockNode::FindFirstChild(const char *key, KeyvalueNodeType type) const
{
	if (!key || !*key)
		return std::shared_ptr<CKeyvalueNode>();

	for (Children::const_iterator it = m_Children.begin(); it != m_Children.end(); it++)
	{
		if ((*it)->GetType() == type && !strcmp(key, (*it)->GetKey().Data()))
			return *it;
	}

	return std::shared_ptr<CKeyvalueNode>();
}

CString CKvBlockNode::FindFirstKeyvalue(const char *key) const
{
	if (key && *key)
	{
		for (Children::const_iterator it = m_Children.begin(); it != m_Children.end(); it++)
		{
			if ((*it)->GetType() == KEYVALUE_NODE_VALUE && !strcmp(key, (*it)->GetKey().Data()))
				return static_cast<CKeyvalue *>(it->get())->GetValue();
		}
	}

	return CString("");
}

CKeyvalues::CKeyvalues()
{
}

CKeyvalues::CKeyvalues(std::shared_ptr<CKvBlockNode> root)
{
	SetRoot(root);
}

void CKeyvalues::SetRoot(std::shared_ptr<CKvBlockNode> root)
{
	m_pRoot = root;
}

void CKeyvalues::Reset(void)
{
	m_pRoot.reset();
}
