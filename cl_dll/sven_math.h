#ifndef SVEN_MATH_H
#define SVEN_MATH_H

#include <math.h>

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define POINT_ON_PLANE			0
#define POINT_IN_FRONT_OF_PLANE	1
#define POINT_BEHIND_PLANE		2

#define PLANE_EPSILON	0.01f

class VECTOR3D
{
public:
	VECTOR3D(void) : x(0.0f), y(0.0f), z(0.0f)
	{
	}

	VECTOR3D(float newX, float newY, float newZ) : x(newX), y(newY), z(newZ)
	{
	}

	VECTOR3D(const float *rhs) : x(rhs[0]), y(rhs[1]), z(rhs[2])
	{
	}

	VECTOR3D(const VECTOR3D &rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
	{
	}

	~VECTOR3D()
	{
	}

	VECTOR3D &operator=(const VECTOR3D &rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return *this;
	}

	void LoadZero(void)
	{
		x = y = z = 0.0f;
	}

	VECTOR3D CrossProduct(const VECTOR3D &rhs) const
	{
		return VECTOR3D(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
	}

	// Parenthesized because cl_util.h defines DotProduct as a macro
	float (DotProduct)(const VECTOR3D &rhs) const
	{
		return x * rhs.x + y * rhs.y + z * rhs.z;
	}

	float GetLength(void) const
	{
		return (float)sqrt((x * x) + (y * y) + (z * z));
	}

	void Normalize(void);
	VECTOR3D GetNormalized(void) const;
	VECTOR3D GetRotatedAxis(double angle, const VECTOR3D &axis) const;

	VECTOR3D operator+(const VECTOR3D &rhs) const
	{
		return VECTOR3D(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	VECTOR3D operator-(const VECTOR3D &rhs) const
	{
		return VECTOR3D(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	VECTOR3D operator*(const float rhs) const
	{
		return VECTOR3D(x * rhs, y * rhs, z * rhs);
	}

	void operator/=(const float rhs)
	{
		if (rhs == 0.0f)
			return;

		x /= rhs;
		y /= rhs;
		z /= rhs;
	}

	VECTOR3D operator-(void) const
	{
		return VECTOR3D(-x, -y, -z);
	}

	operator float *() const
	{
		return (float *)this;
	}

	float	x;
	float	y;
	float	z;
};

class PLANE
{
public:
	PLANE() : normal(VECTOR3D(0.0f, 0.0f, 0.0f)), intercept(0.0f)
	{
	}

	void CalculateIntercept(const VECTOR3D &pointOnPlane)
	{
		intercept = -(normal.DotProduct)(pointOnPlane);
	}

	void Normalize(void);
	int ClassifyPoint(const VECTOR3D &point) const;

	VECTOR3D	normal;
	float		intercept;
};

class COLOR
{
public:
	COLOR(void) : r(0.0f), g(0.0f), b(0.0f), a(0.0f)
	{
	}

	COLOR(float newR, float newG, float newB, float newA) : r(newR), g(newG), b(newB), a(newA)
	{
	}

	~COLOR()
	{
	}

	float	r;
	float	g;
	float	b;
	float	a;
};

class MATRIX4X4
{
public:
	MATRIX4X4(void)
	{
		LoadIdentity();
	}

	~MATRIX4X4()
	{
	}

	void LoadIdentity(void);
	void LoadZero(void);
	void SetPerspective(float left, float right, float bottom, float top, float zNear, float zFar);

	float	entries[16];
};

void _AngleVectors(const VECTOR3D &angles, VECTOR3D *forward, VECTOR3D *right, VECTOR3D *up);

#endif
