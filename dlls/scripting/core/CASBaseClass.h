#ifndef CASBASECLASS_H
#define CASBASECLASS_H

class CASBaseClass
{
public:
	CASBaseClass();
	~CASBaseClass();
};

class CASRefCountedBaseClass : public CASBaseClass
{
public:
	CASRefCountedBaseClass() : m_iRefCount(1) {}

	void AddRef(void) const;
	int GetRefCount(void) const;

protected:
	bool InternalRelease(void) const;

private:
	mutable int	m_iRefCount;
};

class CASAtomicRefCountedBaseClass : public CASBaseClass
{
public:
	CASAtomicRefCountedBaseClass() : m_iRefCount(1) {}

	void AddRef(void) const;

protected:
	bool InternalRelease(void) const;

private:
	mutable int	m_iRefCount;
};

#endif
