#ifndef CASSERVERIMPLEMENTATIONFACTORY_H
#define CASSERVERIMPLEMENTATIONFACTORY_H

#include "scripting/core/IASImplementationFactory.h"

class CASServerImplementationFactory : public IASImplementationFactory
{
public:
	virtual ~CASServerImplementationFactory();

	virtual CASBaseRegistration *CreateRegistration(void) const;
	virtual const char *GetLogFilename(void) const;
};

#endif
