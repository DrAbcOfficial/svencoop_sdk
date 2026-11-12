#ifndef CASREFLECTION_H
#define CASREFLECTION_H

#include <stdarg.h>
#include <sstream>
#include "scripting/core/CASArgument.h"

class CASArguments;
class CASModule;
class CString;

const char *PrimitiveTypeIdToString(int typeId);

namespace CASReflection
{
	bool SetPrimitiveArgument(ArgumentValue_t &value, int typeId, void *argument, bool &isPrimitive);
	bool SetPrimitiveArgument(CASArgument &argument, int typeId, void *value, bool &isPrimitive);
	bool SetObjectArgument(asIScriptEngine *engine, CASArgument &argument, int typeId, void *value);
	bool SetArgument(asIScriptEngine *engine, CASArgument &argument, int typeId, void *value);
	bool GetReturnValue(CASArgument &argument, int typeId, asDWORD flags, asIScriptContext *context);
	bool GetReturnValue(CASArgument &argument, asIScriptFunction *function, asIScriptContext *context,
	                    asDWORD *flags = NULL);
	bool GetReturnValue(void *value, int typeId, asDWORD flags, asIScriptContext *context, CASModule *module);
	bool ConvertInputArgToLargest(int typeId, const ArgumentValue_t &value, asINT64 &integerValue, double &realValue);
	bool ConvertInputArgToLargest(const CASArgument *argument, asINT64 &integerValue, double &realValue);
	ArgumentType_t ArgumentTypeFromTypeId(int typeId, asDWORD flags);
	bool GetArgumentFromVarargs(ArgumentValue_t &value, int typeId, asDWORD flags, va_list &args,
	                            asDWORD *argumentFlags, ArgumentType_t *argumentType);
	bool ConvertEnumToPrimitive(const CASArgument &argument, int typeId, ArgumentValue_t &value);
	bool ConvertPrimitiveToEnum(const CASArgument &argument, ArgumentValue_t &value);
	bool CreateFunctionSignature(asIScriptEngine *engine, std::stringstream &stream, const CString &name,
	                             CASArguments &arguments, unsigned int firstArgument, asIScriptGeneric *generic);
	bool SetContextArgument(asIScriptEngine *engine, asIScriptFunction *function, asIScriptContext *context,
	                        unsigned int index, int typeId, ArgumentValue_t &value, bool varargs);
	bool SetContextArgument(asIScriptEngine *engine, asIScriptFunction *function, asIScriptContext *context,
	                        unsigned int index, CASArgument &argument);
	bool SetContextArgument(asIScriptEngine *engine, asIScriptFunction *function, asIScriptContext *context,
	                        unsigned int index, va_list &arguments);
	bool SetArguments(asIScriptFunction *function, asIScriptContext *context, CASArguments &arguments);
	bool SetArguments(asIScriptFunction *function, asIScriptContext *context, va_list &arguments);
}

#endif
