#ifndef ENTVARS_TYPES_H
#define ENTVARS_TYPES_H

enum entvartype_s
{
	VAR_INVALID = 0,
	VAR_STRING,
	VAR_VECTOR,
	VAR_FLOAT,
	VAR_INTEGER,
	VAR_EDICT,
	VAR_BYTE4,
	VAR_BYTE2,
	VAR_CUSTOM
};

enum { MAX_ENTVAR_VALUE_TEXT = 128, ENTVAR_VECTOR_COMPONENTS = 3 };

const char *EntVarStr(entvartype_s type);
bool UTIL_IsStringInt(const char *text);
bool UTIL_IsStringFloat(const char *text);
bool UTIL_IsString3DVec(const char *text);
entvartype_s UTIL_FigureVarType(const char *text);
bool UTIL_StringToVectorND(float *values, int count, const char *text, char separator);
bool UTIL_StringToVector(float *values, const char *text, char separator);

#endif
