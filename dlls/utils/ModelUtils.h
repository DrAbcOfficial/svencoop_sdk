#ifndef MODELUTILS_H
#define MODELUTILS_H

class IEngineServer;

extern IEngineServer *g_pEngineServer;

void *GetModelPtr(int modelIndex);
int SetBodygroupInt(void *model, int body, int group, int value);
int GetBodygroupInt(void *model, int body, int group);
int GetBodygroupSubmodelCountInt(void *model, int group);
int GetAttachmentCount(void *model);

#endif
