#include "extdll.h"
#include "util.h"
#include "ASVector.h"
#include "CASDocumentation.h"
#include "CString.h"
#include <new>

static const size_t VECTOR_STRING_LENGTH = 512;

void ConstructVector2D(void *memory)
{
	new(memory) Vector2D;
}

void CopyConstructVector2D(const Vector2D &vector, void *memory)
{
	new(memory) Vector2D(vector);
}

void Float2ConstructVector2D(float x, float y, void *memory)
{
	new(memory) Vector2D(x, y);
}

void DestructVector2D(Vector2D *vector)
{
	vector->~Vector2D();
}

void ConstructVector3D(void *memory)
{
	new(memory) Vector(0, 0, 0);
}

void CopyConstructVector3D(const Vector &vector, void *memory)
{
	new(memory) Vector(vector);
}

void Float3ConstructVector3D(float x, float y, float z, void *memory)
{
	new(memory) Vector(x, y, z);
}

void DestructVector3D(Vector *vector)
{
	vector->~Vector();
}

bool Vector2D_Equals(const Vector2D &other, const Vector2D *vector)
{
	return vector->x == other.x && vector->y == other.y;
}

bool Vector_Equals(const Vector &other, const Vector *vector)
{
	return *vector == other;
}

Vector2D Vector2D_Divide(float divisor, const Vector2D *vector)
{
	float reciprocal = 1.0f / divisor;

	return Vector2D(reciprocal * vector->x, reciprocal * vector->y);
}

Vector2D Vector2D_Normalize(Vector2D *vector)
{
	// The script method is const, but this also normalizes its receiver
	*vector = vector->Normalize();
	return *vector;
}

Vector Vector_Multiply(const Vector &other, const Vector *vector)
{
	return Vector(other.x * vector->x, other.y * vector->y, other.z * vector->z);
}

Vector Vector_Divide(const Vector &other, const Vector *vector)
{
	return Vector(vector->x / other.x, vector->y / other.y, vector->z / other.z);
}

float &Vector3D_opIndex(size_t index, Vector *vector)
{
	return ((float *)*vector)[index];
}

float Vector3D_opIndex_const(size_t index, const Vector *vector)
{
	return ((const float *)*vector)[index];
}

float Vector2D_DotProduct(const Vector2D &lhs, const Vector2D &rhs)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	float result;

	// Keep x87 precision until the VM's float result boundary
	__asm
	{
		mov ecx, lhs
		mov eax, rhs
		fld dword ptr [ecx]Vector2D.y
		fmul dword ptr [eax]Vector2D.y
		fld dword ptr [ecx]Vector2D.x
		fmul dword ptr [eax]Vector2D.x
		faddp st(1), st(0)
		fstp result
	}

	return result;
#else
	return (float)((long double)lhs.y * rhs.y + (long double)lhs.x * rhs.x);
#endif
}

float Vector3D_DotProduct(const Vector &lhs, const Vector &rhs)
{
#if defined(_MSC_VER) && defined(_M_IX86)
	float result;

	__asm
	{
		mov ecx, lhs
		mov eax, rhs
		fld dword ptr [ecx]Vector.y
		fmul dword ptr [eax]Vector.y
		fld dword ptr [ecx]Vector.x
		fmul dword ptr [eax]Vector.x
		faddp st(1), st(0)
		fld dword ptr [ecx]Vector.z
		fmul dword ptr [eax]Vector.z
		faddp st(1), st(0)
		fstp result
	}

	return result;
#else
	return (float)((long double)lhs.y * rhs.y + (long double)lhs.x * rhs.x + (long double)lhs.z * rhs.z);
#endif
}

CString Vector2D_ToString(const Vector2D *vector)
{
	char text[VECTOR_STRING_LENGTH];

	snprintf(text, sizeof(text), "%f, %f", vector->x, vector->y);
	return CString(text);
}

CString Vector3D_ToString(const Vector *vector)
{
	char text[VECTOR_STRING_LENGTH];

	snprintf(text, sizeof(text), "%f, %f, %f", vector->x, vector->y, vector->z);
	return CString(text);
}

void RegisterSCScriptVector2D(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("2D Vector", "Vector2D", sizeof(Vector2D),
		asOBJ_VALUE | asOBJ_APP_CLASS_C | asOBJ_APP_CLASS_ALLFLOATS);
	documentation->RegisterObjectBehaviour("Default constructs a 2D vector (0, 0)", "Vector2D",
		asBEHAVE_CONSTRUCT, "void Vector2D()", asFUNCTION(ConstructVector2D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructs a 2D vector", "Vector2D",
		asBEHAVE_CONSTRUCT, "void Vector2D(const Vector2D& in other)",
		asFUNCTION(CopyConstructVector2D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Constructs a 2D vector from 2 floats", "Vector2D",
		asBEHAVE_CONSTRUCT, "void Vector2D(float x, float y)",
		asFUNCTION(Float2ConstructVector2D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Vector2D destructor", "Vector2D", asBEHAVE_DESTRUCT,
		"void DestructVector2D()", asFUNCTION(DestructVector2D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Vector2D x variable", "Vector2D", "float x", asOFFSET(Vector2D, x));
	documentation->RegisterObjectProperty("Vector2D y variable", "Vector2D", "float y", asOFFSET(Vector2D, y));
	documentation->RegisterObjectMethod("Assign vector", "Vector2D",
		"Vector2D& opAssign(const Vector2D& in other)",
		asMETHODPR(Vector2D, operator=, (const Vector2D &), Vector2D &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Add vectors", "Vector2D",
		"Vector2D opAdd(const Vector2D& in other) const",
		asMETHOD(Vector2D, operator+), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Subtract vectors", "Vector2D",
		"Vector2D opSub(const Vector2D& in other) const",
		asMETHOD(Vector2D, operator-), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Multiply vector by a value", "Vector2D",
		"Vector2D opMul(float fl) const", asMETHOD(Vector2D, operator*), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Multiply vector by a value", "Vector2D",
		"Vector2D opMul_r(float fl) const", asMETHOD(Vector2D, operator*), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Divide vector by a value", "Vector2D",
		"Vector2D opDiv(float fl) const", asFUNCTION(Vector2D_Divide), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Divide vector by a value", "Vector2D",
		"Vector2D opDiv_r(float fl) const", asFUNCTION(Vector2D_Divide), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Compare vectors", "Vector2D",
		"bool opEquals(const Vector2D& in other) const", asFUNCTION(Vector2D_Equals), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Gets the length of this vector", "Vector2D",
		"float Length() const", asMETHOD(Vector2D, Length), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the normalized form of this vector", "Vector2D",
		"Vector2D Normalize() const", asFUNCTION(Vector2D_Normalize), asCALL_CDECL_OBJLAST);
	documentation->RegisterGlobalFunction("Returns a dot product of the given vectors",
		"float DotProduct(const Vector2D& in lhs, const Vector2D& in rhs)",
		asFUNCTION(Vector2D_DotProduct), asCALL_CDECL);
	documentation->RegisterObjectMethod("Returns a string representation of this vector", "Vector2D",
		"string ToString() const", asFUNCTION(Vector2D_ToString), asCALL_CDECL_OBJLAST);
}

void RegisterSCScriptVector3D(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("3D Vector", "Vector", sizeof(Vector),
		asOBJ_VALUE | asOBJ_APP_CLASS_CK | asOBJ_APP_CLASS_ALLFLOATS);
	documentation->RegisterObjectBehaviour("Default constructs a 3D vector (0, 0, 0)", "Vector",
		asBEHAVE_CONSTRUCT, "void Vector()", asFUNCTION(ConstructVector3D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Copy constructs a 3D vector", "Vector",
		asBEHAVE_CONSTRUCT, "void Vector(const Vector& in vec)",
		asFUNCTION(CopyConstructVector3D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Constructs a 3D vector from 3 floats", "Vector",
		asBEHAVE_CONSTRUCT, "void Vector(float x, float y, float z)",
		asFUNCTION(Float3ConstructVector3D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectBehaviour("Vector destructor", "Vector", asBEHAVE_DESTRUCT,
		"void DestructVector()", asFUNCTION(DestructVector3D), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectProperty("Vector x variable", "Vector", "float x", asOFFSET(Vector, x));
	documentation->RegisterObjectProperty("Vector y variable", "Vector", "float y", asOFFSET(Vector, y));
	documentation->RegisterObjectProperty("Vector z variable", "Vector", "float z", asOFFSET(Vector, z));
	documentation->RegisterObjectMethod("Assign vector", "Vector",
		"Vector& opAssign(const Vector& in other)",
		asMETHODPR(Vector, operator=, (const Vector &), Vector &), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Negate vector", "Vector", "Vector opNeg() const",
		asMETHODPR(Vector, operator-, () const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Add vectors", "Vector", "Vector opAdd(const Vector& in other) const",
		asMETHOD(Vector, operator+), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Subtract vectors", "Vector", "Vector opSub(const Vector& in other) const",
		asMETHODPR(Vector, operator-, (const Vector &) const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Multiply vector by value", "Vector", "Vector opMul(float fl) const",
		asMETHODPR(Vector, operator*, (float) const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Multiply vector by value", "Vector", "Vector opMul_r(float fl) const",
		asMETHODPR(Vector, operator*, (float) const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Divide vector by value", "Vector", "Vector opDiv(float fl) const",
		asMETHODPR(Vector, operator/, (float) const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Divide vector by value", "Vector", "Vector opDiv_r(float fl) const",
		asMETHODPR(Vector, operator/, (float) const, Vector), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Index operator", "Vector", "float& opIndex(size_t uiIndex)",
		asFUNCTION(Vector3D_opIndex), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Index operator", "Vector", "float opIndex(size_t uiIndex) const",
		asFUNCTION(Vector3D_opIndex_const), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Compare vectors", "Vector", "bool opEquals(const Vector& in other) const",
		asFUNCTION(Vector_Equals), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Multiply vectors", "Vector", "Vector opMul(const Vector& in other) const",
		asFUNCTION(Vector_Multiply), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Divide vectors", "Vector", "Vector opDiv(const Vector& in other) const",
		asFUNCTION(Vector_Divide), asCALL_CDECL_OBJLAST);
	documentation->RegisterObjectMethod("Gets the length of this vector", "Vector", "float Length() const",
		asMETHOD(Vector, Length), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the length of this vector in 2D", "Vector", "float Length2D() const",
		asMETHOD(Vector, Length2D), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the normalized form of this vector", "Vector", "Vector Normalize() const",
		asMETHOD(Vector, Normalize), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns the 2D form of this vector", "Vector", "Vector2D Make2D() const",
		asMETHOD(Vector, Make2D), asCALL_THISCALL);
	documentation->RegisterGlobalFunction("Returns the dot product of the given vectors",
		"float DotProduct(const Vector& in lhs, const Vector& in rhs)", asFUNCTION(Vector3D_DotProduct), asCALL_CDECL);
	documentation->RegisterGlobalFunction("Returns the cross product of the given vectors",
		"Vector CrossProduct(const Vector& in, const Vector& in)",
		asFUNCTIONPR(CrossProduct, (const Vector &, const Vector &), Vector), asCALL_CDECL);
	documentation->RegisterGlobalProperty("Zero vector (0, 0, 0)", "const Vector g_vecZero", (void *)&g_vecZero);
	documentation->RegisterObjectMethod("Returns a string representation of this vector", "Vector",
		"string ToString() const", asFUNCTION(Vector3D_ToString), asCALL_CDECL_OBJLAST);
}
