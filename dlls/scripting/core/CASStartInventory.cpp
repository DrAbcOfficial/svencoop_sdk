#include "scripting/core/CASStartInventory.h"
#include "items/CStartInventory.h"

CASStartInventory::~CASStartInventory()
{
}

bool CASStartInventory::IsAvailable( void ) const
{
	return CStartInventory::GetInstance()->IsAvailable();
}

bool CASStartInventory::Exists( const CString &entry ) const
{
	return CStartInventory::GetInstance()->Exists( entry.Data() );
}

void CASStartInventory::Set( const CString &entry, unsigned long amount )
{
	CStartInventory::GetInstance()->Set( entry.Data(), amount );
}

void CASStartInventory::Set( const CString &entry )
{
	CStartInventory::GetInstance()->Set( entry.Data() );
}

void CASStartInventory::Add( const CString &entry, unsigned long amount, bool add )
{
	CStartInventory::GetInstance()->Add( entry.Data(), amount, add );
}

void CASStartInventory::Remove( const CString &entry )
{
	CStartInventory::GetInstance()->Remove( entry.Data() );
}

bool CASStartInventory::Restock( const CString &entry, unsigned long amount )
{
	return CStartInventory::GetInstance()->Restock( entry.Data(), amount );
}

bool CASStartInventory::Limit( const CString &entry, unsigned long amount )
{
	return CStartInventory::GetInstance()->Limit( entry.Data(), amount );
}

unsigned long CASStartInventory::GetAmount( const CString &entry ) const
{
	return CStartInventory::GetInstance()->GetAmount( entry.Data() );
}
