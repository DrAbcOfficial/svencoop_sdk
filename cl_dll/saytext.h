#ifndef HUD_SAYTEXT_H
#define HUD_SAYTEXT_H

#define MAX_TEXT_LINES 12
#define MAX_TEXT_LINE_LENGTH 256

class CHudBaseTextBlock : public CHudBase
{
public:
	CHudBaseTextBlock(bool blockMuted, int lineStart);
	int Init(void);
	int VidInit(void);
	int Draw(float flTime);
	void InitHUDData(void);
	virtual void FlushText(void);
	virtual float GetScrollTime(void) const = 0;
	virtual void PrintText(int line, const char *text, int size, int clientIndex) = 0;
	virtual void UpdatePos(void) = 0;
	void Print(const char *text, int size, int clientIndex = -1);
	void SetNumLines(int lines);
	int ScrollTextUp(void);
	void FitInBlock(int line);

protected:
	bool	 m_bBlockMuted;
	char	 m_szLineBuffer[MAX_TEXT_LINES + 1][MAX_TEXT_LINE_LENGTH];
	float	*m_pflNameColors[MAX_TEXT_LINES + 1];
	int		 m_iNameLengths[MAX_TEXT_LINES + 1];

	float	 m_flScrollTime;
	int		 m_iX;
	int		 m_iY;
	int		 m_iNumLines;
	int		 m_iLineHeight;
	float	 m_flScrollInterval;
};

class CHudSayText : public CHudBaseTextBlock
{
public:
	CHudSayText(void);
	int Init(void);
	int Draw(float flTime);
	int MsgFunc_SayText(const char *pszName, int iSize, void *pbuf);
	float GetScrollTime(void) const;
	void PrintText(int line, const char *text, int size, int clientIndex);
	void UpdatePos(void);
	void GetChatInputPosition(int *x, int *y);

private:
	cvar_t *m_HUD_saytext;
	cvar_t *m_HUD_saytext_time;
	cvar_t *m_HUD_saytext_lines;
	cvar_t *m_HUD_saytext_block_muted;
};

class CHudNotifyText : public CHudBaseTextBlock
{
public:
	CHudNotifyText(void);
	int Init(void);
	int MsgFunc_NotifyText(const char *pszName, int iSize, void *pbuf);
	float GetScrollTime(void) const;
	void PrintText(int line, const char *text, int size, int clientIndex);
	void UpdatePos(void);
};

#endif
