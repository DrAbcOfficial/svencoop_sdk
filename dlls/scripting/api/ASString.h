#ifndef ASSTRING_H
#define ASSTRING_H

class CASDocumentation;
class asIScriptEngine;
class asIScriptGeneric;

void RegisterSCScriptForwardDeclareString(CASDocumentation *documentation);

void RegisterSCScriptASString(asIScriptEngine *engine, CASDocumentation *documentation);
bool ASSPrintf(char *destination, unsigned int size, const char *format, unsigned int firstArgument, asIScriptGeneric *generic);

#endif
