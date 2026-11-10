/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// saytext.cpp
//
// implementation of CHudSayText class
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "cl_entity.h"

#include <string.h>
#include <stdlib.h>

#include "vgui_TeamFortressViewport.h"

#define SERVER_CONSOLE_NAME		"Server Console"
#define SERVER_CONSOLE_LENGTH	( sizeof( SERVER_CONSOLE_NAME ) - 1 )

extern float *GetClientColor(int clientIndex);

DECLARE_MESSAGE(m_SayText, SayText);
DECLARE_MESSAGE(m_NotifyText, NotifyText);

void CopyText(char *destination, const char *source, size_t size)
{
	if (!destination || !source || !size)
		return;

	memset(destination, 0, size);
	strncpy(destination, source, size - 1);
}

int CHud::DrawConsoleString(int x, int y, const char *string)
{
	if (m_pCvarDebug->value > 0)
	{
		int width, height;

		DRAW_DEBUG_CROSS(x, y);
		GetConsoleStringSize(string, &width, &height);
		DRAW_DEBUG_RECT(x, y, width, height);
	}

	return gEngfuncs.pfnDrawConsoleString(x, y, (char *)string);
}

CHudBaseTextBlock::CHudBaseTextBlock(bool blockMuted, int lineStart)
{
	m_pos.x = m_pos.y = 0;
	m_bBlockMuted = blockMuted;
	m_flScrollTime = 0;
	m_iX = lineStart;
	m_iY = 0;
	m_iNumLines = 5;
	m_iLineHeight = 0;
	m_flScrollInterval = 5.0f;
}

int CHudBaseTextBlock::Init(void)
{
	gHUD.AddHudElem(this);
	InitHUDData();
	return 1;
}

int CHudBaseTextBlock::VidInit(void)
{
	m_iFlags |= HUD_INTERMISSION;
	return 1;
}

void CHudBaseTextBlock::InitHUDData(void)
{
	memset(m_szLineBuffer, 0, sizeof(m_szLineBuffer));
	memset(m_pflNameColors, 0, sizeof(m_pflNameColors));
	memset(m_iNameLengths, 0, sizeof(m_iNameLengths));
}

void CHudBaseTextBlock::SetNumLines(int lines)
{
	m_iNumLines = V_min(V_max(lines, 1), MAX_TEXT_LINES);
}

int CHudBaseTextBlock::ScrollTextUp(void)
{
	m_szLineBuffer[m_iNumLines][0] = 0;
	memmove(m_szLineBuffer[0], m_szLineBuffer[1], m_iNumLines * sizeof(m_szLineBuffer[0]));
	memmove(m_pflNameColors, m_pflNameColors + 1, m_iNumLines * sizeof(m_pflNameColors[0]));
	memmove(m_iNameLengths, m_iNameLengths + 1, m_iNumLines * sizeof(m_iNameLengths[0]));

	for (int i = m_iNumLines; i < MAX_TEXT_LINES; ++i)
		m_szLineBuffer[i][0] = 0;

	if (m_szLineBuffer[0][0] == ' ')
	{
		m_szLineBuffer[0][0] = 2;
		return 1 + ScrollTextUp();
	}

	return 1;
}

void CHudBaseTextBlock::FlushText(void)
{
	while (m_szLineBuffer[0][0])
		ScrollTextUp();
}

int CHudBaseTextBlock::Draw(float flTime)
{
	UpdatePos();

	if (gViewPort && !gViewPort->AllowedToPrintText())
		return 1;

	int y = m_iY;

	m_flScrollTime = V_min(m_flScrollTime, flTime + m_flScrollInterval);

	if (flTime >= m_flScrollTime)
	{
		if (m_szLineBuffer[0][0])
		{
			m_flScrollTime = flTime + m_flScrollInterval;
			ScrollTextUp();
		}
		else
		{
			m_iFlags &= ~HUD_ACTIVE;
		}
	}

	float *defaultColor = GetClientColor(-1);

	for (int i = 0; i < m_iNumLines; ++i)
	{
		gEngfuncs.pfnDrawSetTextColor(defaultColor[0], defaultColor[1], defaultColor[2]);

		char *line = m_szLineBuffer[i];

		if (*line)
		{
			if ((*line == 1 || *line == 2 || *line == 3) && m_pflNameColors[i])
			{
				char name[160];

				CopyText(name, line + 1, sizeof(name));
				name[V_min(m_iNameLengths[i], (int)sizeof(name) - 1)] = 0;
				gEngfuncs.pfnDrawSetTextColor(m_pflNameColors[i][0], m_pflNameColors[i][1], m_pflNameColors[i][2]);

				int x = gHUD.DrawConsoleString(m_iX, y, name);

				gEngfuncs.pfnDrawSetTextColor(defaultColor[0], defaultColor[1], defaultColor[2]);
				gHUD.DrawConsoleString(x, y, line + m_iNameLengths[i] + 1);
			}
			else
			{
				gHUD.DrawConsoleString(m_iX, y, line);
			}
		}

		y += m_iLineHeight;
	}

	return 1;
}

void CHudBaseTextBlock::Print(const char *text, int size, int clientIndex)
{
	if (gViewPort && !gViewPort->AllowedToPrintText())
		return;

	const char *levelName = gEngfuncs.pfnGetLevelName();
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();

	if (m_bBlockMuted && levelName && *levelName && player && player->index != clientIndex
		&& GetClientVoiceMgr()->IsPlayerBlocked(clientIndex))
		return;

	int line;

	for (line = 0; line < m_iNumLines; ++line)
	{
		if (!m_szLineBuffer[line][0])
			break;
	}

	if (line == m_iNumLines)
	{
		ScrollTextUp();
		line = m_iNumLines - 1;
	}

	PrintText(line, text, size, clientIndex);
	ConsolePrint(text);
	FitInBlock(line);

	if (line == 0)
	{
		m_flScrollInterval = GetScrollTime();
		m_flScrollTime = gHUD.m_flTime + m_flScrollInterval;
	}

	m_iFlags |= HUD_ACTIVE;
	PlaySound("misc/talk.wav", 1);
}

void CHudBaseTextBlock::FitInBlock(int line)
{
	int width = 0;

	GetConsoleStringSize(m_szLineBuffer[line], &width, &m_iLineHeight);

	if (width + m_iX <= ScreenWidth - 2 * gHUD.GetBorderSize())
		return;

	int length = m_iX;
	char *lastBreak = NULL;

	for (char *p = m_szLineBuffer[line]; *p; ++p)
	{
		if (p[0] == '/' && p[1] == '(')
		{
			p += 2;

			while (*p && *p != ')')
				++p;

			if (*p)
				++p;

			if (!*p)
				break;
		}

		if (*p == ' ' && p != m_szLineBuffer[line])
			lastBreak = p;

		char character[2] = {*p, 0};
		int characterWidth = 0;

		GetConsoleStringSize(character, &characterWidth, &m_iLineHeight);
		length += characterWidth;

		if (length <= ScreenWidth - 2 * gHUD.GetBorderSize())
			continue;

		if (!lastBreak)
			lastBreak = p - 1;

		int nextLine;

		do
		{
			for (nextLine = 0; nextLine < m_iNumLines; ++nextLine)
			{
				if (!m_szLineBuffer[nextLine][0])
					break;
			}

			if (nextLine == m_iNumLines)
				lastBreak -= MAX_TEXT_LINE_LENGTH * ScrollTextUp();
		}
		while (nextLine == m_iNumLines);

		int difference = (int)strlen(m_szLineBuffer[nextLine]) - (int)strlen(lastBreak);

		if (*lastBreak == ' ')
		{
			if (difference <= MAX_TEXT_LINE_LENGTH)
				UTIL_AppendString(m_szLineBuffer[nextLine], lastBreak, MAX_TEXT_LINE_LENGTH);
		}
		else if ((unsigned int)(difference - 2) < MAX_TEXT_LINE_LENGTH)
		{
			UTIL_AppendString(m_szLineBuffer[nextLine], " ", MAX_TEXT_LINE_LENGTH);
			UTIL_AppendString(m_szLineBuffer[nextLine], lastBreak, MAX_TEXT_LINE_LENGTH);
		}

		*lastBreak = 0;
		FitInBlock(nextLine);
		break;
	}
}

CHudSayText::CHudSayText(void) : CHudBaseTextBlock(true, 10)
{
}

int CHudSayText :: Init( void )
{
	CHudBaseTextBlock::Init();

	HOOK_MESSAGE( SayText );

	m_HUD_saytext = CVAR_CREATE( "hud_saytext", "1", FCVAR_CLIENTDLL);
	m_HUD_saytext_time = CVAR_CREATE( "hud_saytext_time", "5", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	m_HUD_saytext_lines = CVAR_CREATE("hud_saytext_lines", "6", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	m_HUD_saytext_block_muted = CVAR_CREATE("hud_saytext_block_muted", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE);
	m_bBlockMuted = m_HUD_saytext_block_muted->value != 0; // is always drawn during an intermission

	return 1;
}

int CHudSayText :: Draw( float flTime )
{
	if (!m_HUD_saytext->value )
		return 1;

	return CHudBaseTextBlock::Draw(flTime);
		}

int CHudSayText :: MsgFunc_SayText( const char *pszName, int iSize, void *pbuf )
{
	m_bBlockMuted = m_HUD_saytext_block_muted->value != 0;
	BEGIN_READ( pbuf, iSize );

	int client_index = READ_BYTE();

	Print( READ_STRING(), iSize - 1,  client_index );

	return 1;
}

float CHudSayText::GetScrollTime(void) const
{
	return m_HUD_saytext_time->value;
}

void CHudSayText::PrintText(int line, const char *text, int size, int clientIndex)
{
	char type = *text;
	int bufferSize = size;
	const char *serverStart = strstr(text, "<");
	const char *serverEnd = strstr(text, "> ");

	if (type == 1 && clientIndex == 0 && (unsigned int)(serverEnd - serverStart) < SERVER_CONSOLE_LENGTH)
		bufferSize += SERVER_CONSOLE_LENGTH - (serverEnd - serverStart);

	m_iNameLengths[line] = 0;
	m_pflNameColors[line] = GetClientColor(-1);

	char *buffer = (char *)malloc(bufferSize);

	if (!buffer)
		return;

	CopyText(buffer, text, bufferSize);

	if (type == 1 || type == 2 || type == 3)
	{
		if (clientIndex != 0)
		{
			gEngfuncs.pfnGetPlayerInfo(clientIndex, &g_PlayerInfoList[clientIndex]);

			const char *name = g_PlayerInfoList[clientIndex].name;
			const char *nameInText = name ? strstr(buffer, name) : NULL;

			if (nameInText)
			{
				m_iNameLengths[line] = nameInText - buffer + strlen(name);
				m_pflNameColors[line] = GetClientColor(clientIndex);
			}
		}
		else
		{
			memset(buffer, 0, bufferSize);
			UTIL_AppendString(buffer, text, serverStart - text);
			UTIL_AppendString(buffer, SERVER_CONSOLE_NAME, bufferSize);
			UTIL_AppendString(buffer, ": ", bufferSize);
			UTIL_AppendString(buffer, serverEnd + 2, bufferSize);
			memmove(buffer + 1, buffer, bufferSize - 2);
			buffer[bufferSize - 1] = 0;
			buffer[0] = type;

			const char *nameInText = strstr(buffer, SERVER_CONSOLE_NAME);

			if (nameInText)
			{
				m_iNameLengths[line] = nameInText - buffer + SERVER_CONSOLE_LENGTH;
				m_pflNameColors[line] = GetClientColor(0);
			}
		}
	}

	CopyText(m_szLineBuffer[line], buffer, MAX_TEXT_LINE_LENGTH);
	free(buffer);
}

void CHudSayText::UpdatePos(void)
{
	if ((float)m_iLineHeight != m_HUD_saytext_lines->value)
		SetNumLines((int)m_HUD_saytext_lines->value);

	int height = m_iLineHeight * (m_iNumLines + 1);

	m_iX = gHUD.GetBorderSize();
	m_iY = m_iLineHeight + ScreenHeight / 2 + (ScreenHeight / 2 - height) / 2;

	if (height + m_iY > gHUD.GetLineY(2))
		m_iY = gHUD.GetLineY(2) - height;
}

void CHudSayText::GetChatInputPosition(int *x, int *y)
{
	int lines = m_iNumLines;

	UpdatePos();
	*y = m_iY + m_iLineHeight * lines;
	UpdatePos();
	*x = m_iX;
}

CHudNotifyText::CHudNotifyText(void) : CHudBaseTextBlock(false, 10)
{
}

int CHudNotifyText::Init(void)
{
	CHudBaseTextBlock::Init();
	HOOK_MESSAGE(NotifyText);
	return 1;
}

int CHudNotifyText::MsgFunc_NotifyText(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int clientIndex = READ_BYTE();

	Print(READ_STRING(), iSize - 1, clientIndex);
	return 1;
}

float CHudNotifyText::GetScrollTime(void) const
{
	return 5.0f;
}

void CHudNotifyText::PrintText(int line, const char *text, int size, int clientIndex)
{
	m_iNameLengths[line] = 0;
	m_pflNameColors[line] = NULL;
	CopyText(m_szLineBuffer[line], text, MAX_TEXT_LINE_LENGTH);
}

void CHudNotifyText::UpdatePos(void)
{
	m_iX = gHUD.GetBorderSize();
	m_iY = V_max(m_iX, m_iLineHeight);
}
