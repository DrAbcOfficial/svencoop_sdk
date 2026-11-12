#ifndef RGBA_H
#define RGBA_H

class CString;
class Vector;

struct RGBA
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

unsigned char RGBAComponentFromFloat(float value);
void ConstructRGBA(RGBA *color);
void ConstructRGBAComp(unsigned char r, unsigned char g, unsigned char b, unsigned char a, RGBA *color);
void ConstructRGBAFromVector(const Vector &components, unsigned char a, RGBA *color);
void CopyConstructRGBA(const RGBA &other, void *memory);
void DestructRGBA(void *memory);
RGBA &AssignRGBA(const RGBA &other, RGBA *color);
bool RGBA_Equals(const RGBA &other, const RGBA *color);
CString RGBA_ToString(bool withAlpha, const RGBA *color);

#endif
