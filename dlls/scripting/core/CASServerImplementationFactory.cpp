#include "scripting/core/CASServerImplementationFactory.h"
#include "scripting/core/CASServerRegistration.h"

CASServerImplementationFactory::~CASServerImplementationFactory()
{
}

CASBaseRegistration *CASServerImplementationFactory::CreateRegistration(void) const
{
	return new CASServerRegistration;
}

const char *CASServerImplementationFactory::GetLogFilename(void) const
{
	return "AS_server";
}
