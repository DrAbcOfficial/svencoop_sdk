#include "hud.h"
#include "cl_util.h"
#include "client_messagebox.h"
#include "SDL2/SDL_messagebox.h"
#include "SDL2/SDL_error.h"

int UTIL_MessageBox(const char *pszMessage, const char *pszTitle, int iIcon, int iButtons)
{
	if (!pszMessage || !pszMessage[0])
		return 0;

	size_t messageSize = strlen(pszMessage) + 2;
	char *pszText = new char[messageSize];

	memset(pszText, 0, messageSize);
	sprintf(pszText, "%s\n", pszMessage);

	size_t titleSize;
	char *pszCaption;

	if (pszTitle && pszTitle[0])
	{
		titleSize = strlen(pszTitle) + 1;
	}
	else
	{
		titleSize = 32;
		if (iIcon & SDL_MESSAGEBOX_INFORMATION)
			pszTitle = "Information";
		else if (iIcon & SDL_MESSAGEBOX_WARNING)
			pszTitle = "Warning";
		else if (iIcon & SDL_MESSAGEBOX_ERROR)
			pszTitle = "Error";
		else
			pszTitle = "Message";
	}

	pszCaption = new char[titleSize];
	memset(pszCaption, 0, titleSize);
	strcpy(pszCaption, pszTitle);

	SDL_MessageBoxData messageBox = {};
	SDL_MessageBoxButtonData *pButtons;

	messageBox.flags = iIcon;
	messageBox.title = pszCaption;
	messageBox.message = pszText;

	// Test the selectors as masks, in this order
	if (iButtons & MSGBOX_OKCANCEL)
	{
		messageBox.numbuttons = 2;
		pButtons = new SDL_MessageBoxButtonData[2];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDCANCEL;
		pButtons[0].text = "Cancel";
		pButtons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		pButtons[1].buttonid = MSGBOX_IDOK;
		pButtons[1].text = "OK";
	}
	else if (iButtons & MSGBOX_ABORTRETRYIGNORE)
	{
		messageBox.numbuttons = 3;
		pButtons = new SDL_MessageBoxButtonData[3];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDABORT;
		pButtons[0].text = "Abort";
		pButtons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		pButtons[1].buttonid = MSGBOX_IDRETRY;
		pButtons[1].text = "Retry";
		pButtons[2].flags = 0;
		pButtons[2].buttonid = MSGBOX_IDIGNORE;
		pButtons[2].text = "Ignore";
	}
	else if (iButtons & MSGBOX_YESNOCANCEL)
	{
		messageBox.numbuttons = 3;
		pButtons = new SDL_MessageBoxButtonData[3];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDCANCEL;
		pButtons[0].text = "Cancel";
		pButtons[1].flags = 0;
		pButtons[1].buttonid = MSGBOX_IDNO;
		pButtons[1].text = "No";
		pButtons[2].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		pButtons[2].buttonid = MSGBOX_IDYES;
		pButtons[2].text = "Yes";
	}
	else if (iButtons & MSGBOX_YESNO)
	{
		messageBox.numbuttons = 2;
		pButtons = new SDL_MessageBoxButtonData[2];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDNO;
		pButtons[0].text = "No";
		pButtons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		pButtons[1].buttonid = MSGBOX_IDYES;
		pButtons[1].text = "Yes";
	}
	else if (iButtons & MSGBOX_RETRYCANCEL)
	{
		messageBox.numbuttons = 2;
		pButtons = new SDL_MessageBoxButtonData[2];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDCANCEL;
		pButtons[0].text = "Cancel";
		pButtons[1].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
		pButtons[1].buttonid = MSGBOX_IDRETRY;
		pButtons[1].text = "Retry";
	}
	else
	{
		messageBox.numbuttons = 1;
		pButtons = new SDL_MessageBoxButtonData[1];
		pButtons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT | SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
		pButtons[0].buttonid = MSGBOX_IDOK;
		pButtons[0].text = "OK";
	}

	int iResult = MSGBOX_IDCLOSE;

	messageBox.buttons = pButtons;
	if (SDL_ShowMessageBox(&messageBox, &iResult) < 0)
	{
		const char *pszError = SDL_GetError();

		if (CVAR_GET_FLOAT("developer") >= 1)
			gEngfuncs.Con_DPrintf("Failed to show message box: %s\n", pszError);
	}

	delete[] pszText;
	delete[] pszCaption;
	delete[] pButtons;
	return iResult;
}
