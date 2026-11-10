#include "extdll.h"
#include "RGBA.h"
#include "CString.h"
#include <new>

#if defined(_M_IX86) || defined(__SSE__)
#include <xmmintrin.h>
#endif

static const size_t RGBA_STRING_LENGTH = 512;

unsigned char RGBAComponentFromFloat(float value)
{
#if defined(_M_IX86) || defined(__SSE__)
	return (unsigned char)_mm_cvtt_ss2si(_mm_set_ss(value));
#else
	static const float INTEGER_CONVERSION_BOUNDARY = 2147483648.0f;

	if (!(value >= -INTEGER_CONVERSION_BOUNDARY && value < INTEGER_CONVERSION_BOUNDARY))
		return 0;

	return (unsigned char)(int)value;
#endif
}

void ConstructRGBA(RGBA *color)
{
	new(color) RGBA;
	color->r = 0;
	color->g = 0;
	color->b = 0;
	color->a = 0;
}

void ConstructRGBAComp(unsigned char r, unsigned char g, unsigned char b, unsigned char a, RGBA *color)
{
	new(color) RGBA;
	color->r = r;
	color->g = g;
	color->b = b;
	color->a = a;
}

void ConstructRGBAFromVector(const Vector &components, unsigned char a, RGBA *color)
{
	new(color) RGBA;
	color->r = RGBAComponentFromFloat(components.x);
	color->g = RGBAComponentFromFloat(components.y);
	color->b = RGBAComponentFromFloat(components.z);
	color->a = a;
}

void CopyConstructRGBA(const RGBA &other, void *memory)
{
	new(memory) RGBA(other);
}

void DestructRGBA(void *memory)
{
	((RGBA *)memory)->~RGBA();
}

RGBA &AssignRGBA(const RGBA &other, RGBA *color)
{
	*color = other;
	return *color;
}

bool RGBA_Equals(const RGBA &other, const RGBA *color)
{
	return color->r == other.r && color->g == other.g && color->b == other.b && color->a == other.a;
}

CString RGBA_ToString(bool withAlpha, const RGBA *color)
{
	char text[RGBA_STRING_LENGTH];

	if (withAlpha)
		snprintf(text, sizeof(text), "%d %d %d %d", color->r, color->g, color->b, color->a);
	else
		snprintf(text, sizeof(text), "%d %d %d", color->r, color->g, color->b);

	return CString(text);
}
