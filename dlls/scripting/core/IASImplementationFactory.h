#ifndef IASIMPLEMENTATIONFACTORY_H
#define IASIMPLEMENTATIONFACTORY_H

class CASBaseRegistration;

class IASImplementationFactory
{
public:
	virtual ~IASImplementationFactory() = 0;

	virtual CASBaseRegistration *CreateRegistration(void) const = 0;
	virtual const char *GetLogFilename(void) const = 0;
};

#endif
