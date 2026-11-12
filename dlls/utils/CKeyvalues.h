#ifndef CKEYVALUES_H
#define CKEYVALUES_H

#include "CString.h"
#include <memory>
#include <vector>

enum KeyvalueNodeType
{
	KEYVALUE_NODE_VALUE = 0,
	KEYVALUE_NODE_BLOCK
};

class CKeyvalueNode
{
public:
	CKeyvalueNode(KeyvalueNodeType type);
	CKeyvalueNode(const char *key, KeyvalueNodeType type);
	virtual ~CKeyvalueNode();
	virtual void Print(unsigned int level) const = 0;

	void SetKey(const char *key);
	void SetKey(const CString &key);
	const CString &GetKey(void) const { return m_Key; }
	KeyvalueNodeType GetType(void) const { return m_Type; }

private:
	CString				m_Key;
	KeyvalueNodeType	m_Type;
};

class CKeyvalue : public CKeyvalueNode
{
public:
	CKeyvalue();
	CKeyvalue(const char *key, const char *value);
	virtual ~CKeyvalue();
	virtual void Print(unsigned int level) const;

	void SetValue(const char *value);
	void SetValue(const CString &value);
	const CString &GetValue(void) const { return m_Value; }

private:
	CString	m_Value;
};

class CKvBlockNode : public CKeyvalueNode
{
public:
	typedef std::vector<std::shared_ptr<CKeyvalueNode> > Children;

	CKvBlockNode(const char *key);
	CKvBlockNode(const char *key, const Children &children);
	CKvBlockNode(const char *key, std::shared_ptr<CKeyvalueNode> child);
	virtual ~CKvBlockNode();
	virtual void Print(unsigned int level) const;

	void PrintChildren(unsigned int level) const;
	void SetChildren(const Children &children);
	void RemoveAllChildren(void);
	void RemoveAllNotNamed(const char *key);
	std::shared_ptr<CKeyvalueNode> FindFirstChild(const char *key) const;
	std::shared_ptr<CKeyvalueNode> FindFirstChild(const char *key, KeyvalueNodeType type) const;
	CString FindFirstKeyvalue(const char *key) const;
	const Children &GetChildren(void) const { return m_Children; }

private:
	Children	m_Children;
};

class CKeyvalues
{
public:
	CKeyvalues();
	CKeyvalues(std::shared_ptr<CKvBlockNode> root);

	void SetRoot(std::shared_ptr<CKvBlockNode> root);
	void Reset(void);
	const std::shared_ptr<CKvBlockNode> &GetRoot(void) const { return m_pRoot; }

private:
	std::shared_ptr<CKvBlockNode>	m_pRoot;
};

#endif
