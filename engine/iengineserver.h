//
#ifndef IENGINESERVER_H
#define IENGINESERVER_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class IEngineServer : public IBaseInterface
{
public:
	virtual ~IEngineServer() {}

	virtual unsigned char* AddPositionToFatPVS(float* org) = 0;
	virtual unsigned char* AddPositionToFatPAS(float* org) = 0;
	virtual int NumberOfPrecachedModels(void) = 0;
	virtual void* GetModelPtrByIndex(int index) = 0;
	virtual const char* GetAddressOfPlayer( edict_t *pEdict, int includePort ) = 0;
};

#define IENGINESERVER_INTERFACE_VERSION "SCEngineServer002"

#endif // IENGINESERVER_H
