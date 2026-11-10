#ifndef HUD_CUSTOM_H
#define HUD_CUSTOM_H

// channels 0-15 belong to map scripts, 16-31 to plugins
#define HUD_CHANNELS_PER_GROUP	16
#define MAX_HUD_CHANNELS		( HUD_CHANNELS_PER_GROUP * 2 )

enum HUD_ELEMENT
{
	HUD_ELEMENT_SPRITE = 1,
	HUD_ELEMENT_NUMBER,
	HUD_ELEMENT_TIME
};

class CHudBaseElement
{
public:
	virtual bool Init(void);
	virtual bool Setup(void);
	virtual bool Draw(float time);
	virtual HUD_ELEMENT GetType(void) = 0;
	virtual int GetWidth(void) = 0;
	virtual int GetHeight(void) = 0;
	virtual int GetFinalPosX(void);
	virtual int GetFinalPosY(void);

	RGBA GetColor(float time);

	bool	m_bVisible;
	float	m_flStartTime;
	int		m_iFlags;
	int		m_iDrawFlags;
	float	m_flX;
	float	m_flY;
	float	m_flFade;
	RGBA	m_Color;
	RGBA	m_Color2;
	float	m_flFadeIn;
	float	m_flFadeOut;
	float	m_flHoldTime;
	float	m_flFXTime;
	byte	m_iEffect;
};

class CHudCustomSprite : public CHudBaseElement
{
public:
	bool Init(void);
	bool Setup(void);
	bool Draw(float time);
	HUD_ELEMENT GetType(void);
	int GetWidth(void);
	int GetHeight(void);
	SPR_MODE GetDrawMode(void);

	HSPRITE		m_hSprite;
	wrect_t		m_Rect;
	SPR_MODE	m_DrawMode;
	byte		m_iStartFrame;
	byte		m_iNumFrames;
	float		m_flFrameRate;
};

class CHudNumDisplay : public CHudBaseElement
{
public:
	bool Init(void);
	bool Setup(void);
	bool Draw(float time);
	HUD_ELEMENT GetType(void);
	int GetWidth(void);
	int GetHeight(void);
	int TransformFlags(int hudFlags);

	HSPRITE	m_hSprite;
	wrect_t	m_Rect;
	float	m_flValue;
	int		m_iMinDigits;
	int		m_iMaxDigits;
};

class CHudTimeDisplay : public CHudBaseElement
{
public:
	bool Init(void);
	bool Setup(void);
	bool Draw(float time);
	HUD_ELEMENT GetType(void);
	int GetWidth(void);
	int GetHeight(void);
	void UpdateTime(float time);

	HSPRITE	m_hSprite;
	wrect_t	m_Rect;
	int		m_iColon;
	int		m_iDot;
	int		m_iColonWidth;
	int		m_iColonHeight;
	float	m_flValue;
	int		m_iHours;
	int		m_iMinutes;
	int		m_iSeconds;
	int		m_iMilliseconds;
	float	m_flUpdateTime;
};

class CHudCustom : public CHudBase
{
public:
	int Init(void);
	int VidInit(void);
	int Draw(float time);
	void Reset(void);
	void Shutdown(void);
	void DrawChannelGroup(int group, float time);
	CHudBaseElement *ElementFactory(HUD_ELEMENT type);
	CHudBaseElement *GetChannel(byte channel);
	CHudBaseElement *AssignChannel(byte channel, HUD_ELEMENT type);
	void ClearChannel(byte channel);
	void ClearAll(void);

	int MsgFunc_ToggleElem(const char *name, int size, void *buffer);
	int MsgFunc_CustSpr(const char *name, int size, void *buffer);
	int MsgFunc_NumDisplay(const char *name, int size, void *buffer);
	int MsgFunc_UpdateNum(const char *name, int size, void *buffer);
	int MsgFunc_TimeDisplay(const char *name, int size, void *buffer);
	int MsgFunc_UpdateTime(const char *name, int size, void *buffer);

private:
	CHudBaseElement *m_pChannels[MAX_HUD_CHANNELS];
};

void ScaleColors(RGBA &color);
RGBA ColorGrad(const RGBA &first, const RGBA &second, float fraction, bool reverse);

#endif
