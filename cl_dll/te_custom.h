#ifndef TE_CUSTOM_H
#define TE_CUSTOM_H

// TE_CUSTOM message types
#define TE_CUSTOM_HWGRUNT_FIRE	1
#define TE_CUSTOM_XEN_PORTAL	2
#define TE_CUSTOM_GILDED_UZI	3

extern bool g_fCanHaveGildedUzis;

void TE_CUSTOM_Message(const char *pszName, int iSize, void *pbuf);

#endif
