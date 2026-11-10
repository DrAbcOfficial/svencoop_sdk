#include "sven_math.h"

#include <string.h>

// SetPerspective treats this far distance as an infinite far plane
#define MATRIX_INFINITE_FAR		-1.0f
#define MATRIX_INFINITE_NUDGE	0.999f

void VECTOR3D::Normalize(void)
{
	float length = GetLength();

	if (length == 1 || length == 0)
		return;

	float scale = 1.0f / length;

	x *= scale;
	y *= scale;
	z *= scale;
}

VECTOR3D VECTOR3D::GetNormalized(void) const
{
	VECTOR3D result(*this);

	result.Normalize();
	return result;
}

VECTOR3D VECTOR3D::GetRotatedAxis(double angle, const VECTOR3D &axis) const
{
	if (angle == 0.0)
		return *this;

	VECTOR3D u = axis.GetNormalized();
	VECTOR3D row0, row1, row2;
	float sine = (float)sin(M_PI * angle / 180);
	float cosine = (float)cos(M_PI * angle / 180);
	float oneMinusCosine = 1.0f - cosine;

	row0.x = u.x * u.x + cosine * (1 - u.x * u.x);
	row0.y = u.x * u.y * oneMinusCosine - sine * u.z;
	row0.z = u.x * u.z * oneMinusCosine + sine * u.y;

	row1.x = u.x * u.y * oneMinusCosine + sine * u.z;
	row1.y = u.y * u.y + cosine * (1 - u.y * u.y);
	row1.z = u.y * u.z * oneMinusCosine - sine * u.x;

	row2.x = u.x * u.z * oneMinusCosine - sine * u.y;
	row2.y = u.y * u.z * oneMinusCosine + sine * u.x;
	row2.z = u.z * u.z + cosine * (1 - u.z * u.z);

	return VECTOR3D((this->DotProduct)(row0), (this->DotProduct)(row1), (this->DotProduct)(row2));
}

void PLANE::Normalize(void)
{
	float length = normal.GetLength();

	normal /= length;
	intercept /= length;
}

int PLANE::ClassifyPoint(const VECTOR3D &point) const
{
	float distance = point.x * normal.x + point.y * normal.y + point.z * normal.z + intercept;

	if (distance > PLANE_EPSILON)
		return POINT_IN_FRONT_OF_PLANE;

	if (distance < -PLANE_EPSILON)
		return POINT_BEHIND_PLANE;

	return POINT_ON_PLANE;
}

void MATRIX4X4::LoadIdentity(void)
{
	memset(entries, 0, sizeof(entries));

	entries[0] = 1.0f;
	entries[5] = 1.0f;
	entries[10] = 1.0f;
	entries[15] = 1.0f;
}

void MATRIX4X4::LoadZero(void)
{
	memset(entries, 0, sizeof(entries));
}

void MATRIX4X4::SetPerspective(float left, float right, float bottom, float top, float zNear, float zFar)
{
	LoadZero();

	if (left == right || bottom == top || zNear == zFar)
		return;

	entries[0] = (zNear + zNear) / (right - left);
	entries[5] = (zNear + zNear) / (top - bottom);
	entries[8] = (right + left) / (right - left);
	entries[9] = (top + bottom) / (top - bottom);
	entries[11] = -1.0f;

	if (zFar == MATRIX_INFINITE_FAR)
	{
		entries[10] = -MATRIX_INFINITE_NUDGE;
		entries[14] = zNear * -2.0f * MATRIX_INFINITE_NUDGE;
		return;
	}

	entries[10] = -(zNear + zFar) / (zFar - zNear);
	entries[14] = -((zFar + zFar) * zNear) / (zFar - zNear);
}

void _AngleVectors(const VECTOR3D &angles, VECTOR3D *forward, VECTOR3D *right, VECTOR3D *up)
{
	float toRadians = (float)(M_PI / 180);
	float sinYaw = sinf(angles.y * toRadians);
	float cosYaw = cosf(angles.y * toRadians);
	float sinPitch = sinf(angles.x * toRadians);
	float cosPitch = cosf(angles.x * toRadians);
	float sinRoll = sinf(angles.z * toRadians);
	float cosRoll = cosf(angles.z * toRadians);

	if (forward)
	{
		forward->x = cosPitch * cosYaw;
		forward->y = cosPitch * sinYaw;
		forward->z = -sinPitch;
	}

	if (right)
	{
		float rollPitch = sinPitch * -sinRoll;

		right->x = cosRoll * sinYaw + rollPitch * cosYaw;
		right->y = rollPitch * sinYaw - cosRoll * cosYaw;
		right->z = -sinRoll * cosPitch;
	}

	if (up)
	{
		float rollPitch = sinPitch * cosRoll;

		up->x = rollPitch * cosYaw + sinRoll * sinYaw;
		up->y = rollPitch * sinYaw - cosYaw * sinRoll;
		up->z = cosRoll * cosPitch;
	}
}
