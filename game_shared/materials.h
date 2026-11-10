#ifndef MATERIALS_H
#define MATERIALS_H

#define MAX_MATERIALS		4096
#define MAX_MATERIAL_NAME	16

struct materialentry_t
{
	char	name[MAX_MATERIAL_NAME];
	char	type;
};

class CMaterialsList
{
public:
	CMaterialsList();
	~CMaterialsList();

	void Reset(void);
	void LoadMaterialsList(const char *customFile);
	void SortMaterials(void);
	int LinearMaterialSearch(const char *name) const;
	int LookupMaterialIndex(const char *name) const;
	char FindMaterialType(const char *name) const;

	materialentry_t	m_Materials[MAX_MATERIALS];
	int				m_iMaterialCount;
};

#endif
