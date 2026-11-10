#ifndef HUD_INVENTORY_H
#define HUD_INVENTORY_H

class CHudInventoryIcon : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void Reset(void);

	int GetWidth(void);
	int GetHeight(void);
	int GetAlpha(void);
	void Visible(bool visible);
	void SetCount(int count);
	void SetIcon(const char *name);

private:
	HSPRITE	m_hSprite;
	wrect_t	m_rcSprite;
	HSPRITE	m_hCustomSprite;
	wrect_t	m_rcCustomSprite;
	bool	m_bVisible;
	int		m_iCount;
};

#endif
