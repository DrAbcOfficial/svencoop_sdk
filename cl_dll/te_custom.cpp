#include "extdll.h"
#include "hud_iface.h"
#include "te_custom.h"
#include "client_hwfire.h"
#include "client_xenportal.h"
#include "cl_entity.h"
#include "pm_defs.h"
#include "ev_hldm.h"
#include "parsemsg.h"

void AlertMessage(ALERT_TYPE type, char *format, ...);

bool g_fCanHaveGildedUzis;

void TE_CUSTOM_Message(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);

	int iType = READ_BYTE();

	switch (iType)
	{
	case TE_CUSTOM_HWGRUNT_FIRE:
	{
		Vector vecOffset(0, 0, 0);
		short iEntity = READ_SHORT();
		int iTarget = READ_SHORT();

		if (iTarget > 0)
			READ_VECTOR(vecOffset);

		TE_CreateHWGEffect(iEntity, vecOffset, iTarget);
		break;
	}

	case TE_CUSTOM_XEN_PORTAL:
		TE_CreateXenPortal();
		break;

	case TE_CUSTOM_GILDED_UZI:
		g_fCanHaveGildedUzis = READ_BYTE() == 1;
		break;

	default:
		AlertMessage(at_console, "TE_CUSTOM error: unknown type %d\n", iType);
		break;
	}
}
