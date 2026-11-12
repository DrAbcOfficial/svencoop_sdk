#ifndef CSTARTINVENTORY_H
#define CSTARTINVENTORY_H

class CStartInventory
{
public:
	static CStartInventory *GetInstance( void );

	bool IsAvailable( void ) const;
	bool Exists( const char *entry ) const;
	void Set( const char *entry, unsigned long amount );
	void Set( const char *entry );
	bool Add( const char *entry, unsigned long amount, bool add = true );
	void Remove( const char *entry );
	bool Restock( const char *entry, unsigned long amount );
	bool Limit( const char *entry, unsigned long amount );
	unsigned long GetAmount( const char *entry ) const;
};

#endif
