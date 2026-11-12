#include "extdll.h"
#include "studio.h"
#include "iengineserver.h"
#include "utils/ModelUtils.h"

void *GetModelPtr(int modelIndex)
{
	return g_pEngineServer->GetModelPtrByIndex(modelIndex);
}

int SetBodygroupInt(void *model, int body, int group, int value)
{
	studiohdr_t *header = (studiohdr_t *)model;

	if (!header || group < 0 || group > header->numbodyparts || value < 0)
		return 0;

	mstudiobodyparts_t *bodyPart = (mstudiobodyparts_t *)((byte *)header + header->bodypartindex) + group;

	if (value >= bodyPart->nummodels)
		return 0;

	int current = body / bodyPart->base % bodyPart->nummodels;

	return body - current * bodyPart->base + value * bodyPart->base;
}

int GetBodygroupInt(void *model, int body, int group)
{
	studiohdr_t *header = (studiohdr_t *)model;

	if (!header || group < 0 || group > header->numbodyparts)
		return 0;

	mstudiobodyparts_t *bodyPart = (mstudiobodyparts_t *)((byte *)header + header->bodypartindex) + group;

	if (bodyPart->nummodels <= 1)
		return 0;

	return body / bodyPart->base % bodyPart->nummodels;
}

int GetBodygroupSubmodelCountInt(void *model, int group)
{
	studiohdr_t *header = (studiohdr_t *)model;

	if (!header || group < 0 || group > header->numbodyparts)
		return 0;

	mstudiobodyparts_t *bodyPart = (mstudiobodyparts_t *)((byte *)header + header->bodypartindex) + group;

	return bodyPart ? bodyPart->nummodels : 0;
}

int GetAttachmentCount(void *model)
{
	return ((studiohdr_t *)model)->numattachments;
}
