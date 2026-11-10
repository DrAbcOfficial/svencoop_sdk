#include "vgui_int.h"
#include "VGUI_Font.h"

#include "hud.h"
#include "cl_util.h"
#include "client_media.h"
#include "client_filesystem.h"
#include "client_support.h"
#include "vgui_MediaSelect.h"

#include <math.h>
#include <string.h>

class CMenuHandler_Callback : public ActionSignal
{
public:
	CMenuHandler_Callback(void *data, void (*callback)(void *))
	{
		setData(data, callback);
	}

	virtual void actionPerformed(Panel *panel)
	{
		if (m_pCallback)
			m_pCallback(m_pData);
	}

	virtual void setData(void *data, void (*callback)(void *))
	{
		m_pData = data;
		m_pCallback = callback;
	}

private:
	void	*m_pData;
	void	(*m_pCallback)(void *);
};

void MediaButtonCallback(void *data)
{
	MediaPlayer.StopSong();
	MediaPlayer.SetSong(((MediaData *)data)->index);
	gViewPort->m_pMediaSelect->Update();
}

void CopyMediaText(char *destination, const char *source, size_t size)
{
	memset(destination, 0, size);

	if (source && *source)
		strncpy(destination, source, size - 1);
}

void CMediaSelect::Output(void)
{
	FileHandle_t file = g_pFileSystem->Open("media-data.txt", "a", "GAMECONFIG");

	if (!file)
		return;

	char text[16384];
	unsigned int count = MediaPlayer.m_iSongCount;
	int length = _snprintf(text, sizeof(text), "Count: %u {\n", count);

	if (length >= 0 && (unsigned int)length < sizeof(text))
		g_pFileSystem->Write(text, sizeof(text), file);

	for (unsigned int i = 0; i < m_iItems; i++)
	{
		if ((int)i >= m_Items.getCount() || !m_Items[i])
			continue;

		MediaData *data = (MediaData *)m_Items[i]->m_pUserData;
		char path[1024];

		UTIL_CopyString(path, data->path, sizeof(path));
		length = _snprintf(text, sizeof(text), "\tButton: %u Data: Media %d Str: %s\n", count, data->index, path);

		if (length >= 0 && (unsigned int)length < sizeof(text))
			g_pFileSystem->Write(text, sizeof(text), file);
	}

	g_pFileSystem->Write("}\n", sizeof("}\n"), file);
	g_pFileSystem->Close(file);
}

CMediaSelect::CMediaSelect(int iTrans, int iRemoveMe, int x, int y, int wide, int tall)
	: CMenuPanel(iTrans, iRemoveMe, x, y, wide, tall)
{
	CSchemeManager *schemes = gViewPort->GetSchemeManager();
	SchemeHandle_t buttonScheme = schemes->getSchemeHandle("Primary Button Text");
	SchemeHandle_t textScheme = schemes->getSchemeHandle("Scoreboard Text");
	float sx = (float)ScreenWidth / 640.0f;
	float sy = (float)ScreenHeight / 480.0f;
	int r, g, b, a;

	m_iItems = 0;
	m_iActiveItem = 0;
	m_bDirty = true;

	m_pBackground = new CTransparentPanel(200, (int)(sx * 128), (int)(sy * 36),
		(int)(sx * 360), (int)(sy * 404));
	m_pBackground->setParent(this);
	m_pBackground->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pBackground->setVisible(true);

	Label *titleBar = new Label("", 0, 0, (int)(sx * 360), (int)(sy * 24));

	titleBar->setBgColor((Scheme::SchemeColor)4);
	titleBar->setFgColor((Scheme::SchemeColor)3);
	titleBar->setContentAlignment(Label::a_west);
	titleBar->setParent(m_pBackground);

	m_pTitle = new Label("", (int)(sx * 4), (int)(sy * 4), (int)(sx * 332), (int)(sy * 16));
	m_pTitle->setParent(m_pBackground);
	m_pTitle->setFont(schemes->getFont(buttonScheme));
	schemes->getFgColor(buttonScheme, r, g, b, a);
	m_pTitle->setFgColor(r, g, b, a);
	m_pTitle->setBgColor(0, 0, 0, 255);
	m_pTitle->setContentAlignment(Label::a_center);

	if (!MediaPlayer.m_bPlaylistLoaded)
		m_pTitle->setText("Sven Co-op Media Player");

	CImageLabel *closeImage = new CImageLabel("close", (int)sx, (int)sy);

	closeImage->setParent(m_pBackground);
	closeImage->setPos((int)(sx * 341 + (sy * 12 - closeImage->getImageWide()) * 0.5f),
		(int)((sy * 12 - closeImage->getImageTall()) * 0.5f));

	CommandButton *close = new CommandButton("", (int)(sx * 341), 0, (int)(sx * 20), (int)(sx * 16));

	close->setParent(m_pBackground);
	close->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
	close->setArmed(false);
	close->m_bNoHighlight = true;
	close->m_bFlat = true;
	close->m_bNoBorder = true;

	m_pTime = new Label("", (int)(sx * 286), (int)(sy * 42), (int)(sx * 27), (int)(sy * 12));
	m_pTime->setFont(schemes->getFont(textScheme));
	schemes->getFgColor(textScheme, r, g, b, a);
	m_pTime->setFgColor(r, g, b, a);
	m_pTime->setBgColor(0, 0, 0, 255);
	m_pTime->setContentAlignment(Label::a_center);
	m_pTime->setParent(m_pBackground);

	Label *progressBorder = new Label("", (int)(sx * 135), (int)(sy * 31), (int)(sx * 216), (int)(sy * 7));

	progressBorder->setBgColor(0, 0, 0, 255);
	progressBorder->setFgColor(0, 0, 0, 255);
	progressBorder->setParent(m_pBackground);
	progressBorder->setBorder(new LineBorder(Color(115, 150, 180, 0)));

	m_pFill = new Label("", (int)(sx * 135), (int)(sy * 31));
	m_pFill->setBgColor((Scheme::SchemeColor)4);
	m_pFill->setFgColor((Scheme::SchemeColor)3);
	m_pFill->setParent(m_pBackground);
	m_pFill->setVisible(false);

	const char *images[] = { "previous", "play", "stop", "pause", "next" };
	const char *commands[] = { "LastSong", "PlayMedia", "StopSong", "PauseSong", "NextSong" };
	CImageLabel *previous = NULL;

	for (int i = 0; i < 5; i++)
	{
		int left = 10 + i * 24;
		CImageLabel *image = new CImageLabel(images[i], (int)sx, (int)sy);

		image->setParent(m_pBackground);

		if (!previous)
			previous = image;

		image->setPos((int)(sx * left + (sx * 22 - previous->getImageWide()) * 0.5f),
			(int)(sy * 32 + (sy * 22 - previous->getImageTall()) * 0.5f));

		CommandButton *button = new CommandButton("", (int)(sx * left), (int)(sy * 32),
			(int)(sx * 22), (int)(sx * 22));

		button->setParent(m_pBackground);
		button->setContentAlignment(Label::a_center);
		button->addActionSignal(new CMenuHandler_StringCommand(commands[i]));
	}

	const char *smallImages[] = { "rand", "hud", "time", "vup", "vdn" };
	const char *smallCommands[] = { "RandomSong", "MediaHud", "MediaTime", "IncVol", "DecVol" };
	const int imagePositions[] = { 133, 148, 163, 214, 229 };

	for (int i = 0; i < 5; i++)
	{
		int left = imagePositions[i];
		CImageLabel *image = new CImageLabel(smallImages[i], (int)sx, (int)sy);

		image->setParent(m_pBackground);
		image->setPos((int)(sx * left + (sy * 12 - image->getImageWide()) * 0.5f),
			(int)(sy * 39 + (sy * 12 - image->getImageTall()) * 0.5f));

		CommandButton *button = new CommandButton("", (int)(sx * (left + 3)), (int)(sy * 43),
			(int)(sx * 12), (int)(sx * 12));

		button->setParent(m_pBackground);
		button->setContentAlignment(Label::a_center);
		button->setFont(schemes->getFont(textScheme));
		button->addActionSignal(new CMenuHandler_StringCommand(smallCommands[i]));
	}

	m_pVolume = new Label("", (int)(sx * 242), (int)(sy * 42), (int)(sx * 27), (int)(sy * 12));
	m_pVolume->setFont(schemes->getFont(textScheme));
	schemes->getFgColor(textScheme, r, g, b, a);
	m_pVolume->setFgColor(r, g, b, a);
	m_pVolume->setBgColor(0, 0, 0, 255);
	m_pVolume->setContentAlignment(Label::a_center);
	m_pVolume->setParent(m_pBackground);
	m_pVolume->setText("5");

	CImageLabel *reloadImage = new CImageLabel("rand", (int)sx, (int)sy);

	reloadImage->setParent(m_pBackground);
	reloadImage->setPos((int)(sx * 269 + (sy * 12 - reloadImage->getImageWide()) * 0.5f),
		(int)(sy * 39 + (sy * 12 - reloadImage->getImageTall()) * 0.5f));

	CommandButton *reload = new CommandButton("", (int)(sx * 272), (int)(sy * 43),
		(int)(sx * 12), (int)(sx * 12));

	reload->setParent(m_pBackground);
	reload->setContentAlignment(Label::a_center);
	reload->setFont(schemes->getFont(textScheme));
	reload->addActionSignal(new CMenuHandler_StringCommand("RelPls"));

	m_pRandom = new CImageLabel("randon", (int)sx, (int)sy, (int)(sx * 12), (int)(sy * 12));
	m_pRandom->setParent(m_pBackground);
	m_pRandom->setPos((int)(sx * 316 + (sy * 12 - m_pRandom->getImageWide()) * 0.5f),
		(int)(sy * 42 + (sy * 12 - m_pRandom->getImageTall()) * 0.5f));
	m_pRandom->setVisible(false);

	m_pScrollPanel = new CTFScrollPanel((int)(sx * 138), (int)(sy * 118), (int)(sx * 342), (int)(sy * 314));
	m_pScrollPanel->setParent(this);
	m_pScrollPanel->setScrollBarAutoVisible(true, true);
	m_pScrollPanel->setScrollBarVisible(false, false);
	m_pScrollPanel->validate();
	m_pScrollPanel->setVisible(true);

	m_pSelection = new Label("", 0, 0, 1, 1);
	m_pSelection->setBgColor(255, 0, 0, 255);
	m_pSelection->setFgColor(255, 0, 0, 255);
	m_pSelection->setParent(m_pScrollPanel->getClient());
	m_pSelection->setBorder(new LineBorder(Color(115, 150, 180, 0)));
	m_pSelection->setVisible(false);
	Rebuild();
}

CMediaSelect::~CMediaSelect()
{
}

void CMediaSelect::Open(void)
{
	Update();
	CMenuPanel::Open();
}

bool CMediaSelect::SlotInput(int slot)
{
	gViewPort->HideVGUIMenu();
	return true;
}

void CMediaSelect::SetTitleText(const char *text)
{
	char title[512];

	CopyMediaText(title, text, sizeof(title));
	char *name = strrchr(title, '\\');

	m_pTitle->setText(name ? name + 1 : title);
	Update();
}

void CMediaSelect::SetTime(const char *text)
{
	CopyMediaText(m_szTime, text, sizeof(m_szTime));
}

void CMediaSelect::SetFill(float fill)
{
	double value = fminf(25.0f, fill);

	m_flFill = value >= 0 ? (float)value : 0;
}

void CMediaSelect::UpdateTime(void)
{
	if (!MediaPlayer.m_bPlaylistLoaded)
		return;

	m_pTime->setText(m_szTime);

	if (m_flFill > 1)
		m_flFill = 1;

	m_pFill->setSize((int)(((float)ScreenWidth / 640.0f) * (m_flFill * 216.0f)),
		(int)(((float)ScreenHeight / 480.0f) * 7));
	m_pFill->setVisible(true);
}

void CMediaSelect::Update(void)
{
	m_pRandom->setVisible(MediaPlayer.m_bRandom);
	m_pVolume->setText("%u", MediaPlayer.m_iVolume);
	unsigned int song = MediaPlayer.m_iSongIndex;

	if (m_bDirty)
	{
		m_bDirty = false;
		song = (unsigned int)-1;
	}

	int y = 0;

	for (unsigned int i = 0; i < m_iItems; i++)
	{
		CScrollButton *button = m_Items[i];

		if (!button)
			continue;

		button->setParent(m_pScrollPanel);
		int wide = m_pScrollPanel->getWide() - m_pScrollPanel->getVerticalScrollBar()->getWide() - 4;
		int tall = (int)(((float)ScreenHeight / 480.0f) * 15);

		button->setSize(wide, tall);
		button->setPos(0, y);
		button->setVisible(true);
		button->m_bInUse = true;
		button->setParent(m_pScrollPanel->getClient());
		button->setContentAlignment(Label::a_west);
		button->setButtonBorderEnabled(false);
		button->setPaintBorderEnabled(false);
		button->m_bNoBorder = true;

		if (song == i)
		{
			m_pSelection->setPos(0, y);
			m_pSelection->setSize(wide, tall);
			m_pSelection->setVisible(true);
		}

		y += button->getTall();
	}

	m_pScrollPanel->validate();
}

bool CMediaSelect::AddItem(CScrollButton *button)
{
	if (button)
	{
		m_Items.ensureCapacity(m_iItems + 1);
		button->setParent(m_pScrollPanel->getClient());
		m_Items.addElement(button);
		m_iItems++;
		Update();
	}

	return true;
}

bool CMediaSelect::RemoveItem(CScrollButton *button)
{
	if (button)
	{
		int count = m_Items.getCount();

		m_Items.removeElement(button);

		if (count != m_Items.getCount())
		{
			m_iItems--;
			Panel *parent = button->getParent();

			if (parent)
				parent->removeChild(button);

			delete button;
		}

		Update();
	}

	return true;
}

bool CMediaSelect::RemoveItem(unsigned int index)
{
	if (index != (unsigned int)-1 && index < m_iItems)
		return RemoveItem(m_Items[index]);

	return false;
}

void CMediaSelect::RemoveAllItems(void)
{
	while (m_iItems)
		RemoveItem(m_Items.getCount() - 1);
}

CScrollButton *CMediaSelect::GetActiveItem(void)
{
	if (m_iActiveItem == (unsigned int)-1 || m_iActiveItem >= m_iItems)
		return NULL;

	return m_Items[m_iActiveItem];
}

unsigned int CMediaSelect::GetItemIndex(CScrollButton *button)
{
	if (!button)
		return (unsigned int)-1;

	for (unsigned int i = 0; i < m_iItems; i++)
	{
		if (m_Items[i] == button)
			return i;
	}

	return (unsigned int)-1;
}

void CMediaSelect::SetActiveItem(unsigned int index)
{
	if (index != (unsigned int)-1 && index < m_iItems)
		m_iActiveItem = index;

	Update();
}

void CMediaSelect::SetActiveItem(CScrollButton *button)
{
	if (button)
	{
		unsigned int index = GetItemIndex(button);

		if (index != (unsigned int)-1)
			SetActiveItem(index);
	}
}

void CMediaSelect::Rebuild(void)
{
	MediaPlayer.StopMedia();

	if (!MediaPlayer.m_bPlaylistLoaded)
		MediaPlayer.LoadFromFile(NULL);

	RemoveAllItems();
	unsigned int count = MediaPlayer.m_iSongCount;

	if (count)
	{
		unsigned int extra = count - m_iItems;

		m_Items.ensureCapacity(count);

		for (unsigned int i = 0; i < extra; i++)
		{
			CScrollButton *button = new CScrollButton("", 0, 0, 60, 20);
			MediaData *data = new MediaData;

			data->index = 1;
			CopyMediaText(data->path, "", sizeof(data->path));
			operator delete(button->m_pUserData);
			button->m_pUserData = data;
			button->m_pActionSignal = new CMenuHandler_Callback(data, MediaButtonCallback);
			button->addActionSignal(button->m_pActionSignal);
			AddItem(button);
		}
	}

	for (unsigned int i = 0; i < m_iItems; i++)
	{
		CScrollButton *button = m_Items[i];

		if (!button)
			continue;

		const char *path = MediaPlayer.GetSongByIndex(i);
		const char *name = strrchr(path, '\\');

		button->setText(name ? name + 1 : path);
		MediaData *data = new MediaData;

		data->index = i;
		CopyMediaText(data->path, path, sizeof(data->path));
		operator delete(button->m_pUserData);
		button->m_pUserData = data;
		button->m_bInUse = true;
		button->setVisible(false);
		((CMenuHandler_Callback *)button->m_pActionSignal)->setData(data, MediaButtonCallback);

		if (i >= count)
			button->m_bInUse = false;
	}

	MediaPlayer.SetSong((unsigned int)-1);
	m_bDirty = true;
	m_pScrollPanel->validate();
	Update();
}
