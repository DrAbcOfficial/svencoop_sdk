//========= Copyright (c) 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// mathlib.h

struct mplane_s;

typedef float vec_t;
#ifndef DID_VEC3_T_DEFINE
#define DID_VEC3_T_DEFINE
typedef vec_t vec3_t[3];
#endif
typedef vec_t vec4_t[4];	// x,y,z,w
typedef vec_t vec5_t[5];

typedef short vec_s_t;
typedef vec_s_t vec3s_t[3];
typedef vec_s_t vec4s_t[4];	// x,y,z,w
typedef vec_s_t vec5s_t[5];

typedef	int	fixed4_t;
typedef	int	fixed8_t;
typedef	int	fixed16_t;
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define ROLL	2
#define PITCH	0
#define YAW		1

#ifdef __cplusplus
#include <cmath>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern vec3_t vec3_origin;

#ifdef __cplusplus
}
#endif

// std::isnan replaces the old *(int*)&x aliasing-violation nanmask trick.
#define	IS_NAN(x) std::isnan( x )

#ifndef VECTOR_H
	#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#endif

// Use this definition globally
#define	EQUAL_EPSILON	0.001

#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#define VectorClear(a) {(a)[0]=0.0;(a)[1]=0.0;(a)[2]=0.0;}

void VectorMA(const vec_t* veca, float scale, const vec_t* vecb, vec_t* vecc);

vec_t _DotProduct( vec_t* v1, vec_t* v2 );
void _VectorSubtract(vec_t* veca, vec_t* vecb, vec_t* out);
void _VectorAdd( vec_t* veca, vec_t* vecb, vec_t* out );
void _VectorCopy(vec_t* in, vec_t* out);

int VectorCompare( const vec_t* v1, const vec_t* v2 );
float Length( const vec_t* v );
void CrossProduct( const vec_t* v1, const vec_t* v2, vec_t* cross );
float VectorNormalize( vec_t* v );		// returns vector length
void VectorInverse( vec_t* v );
void VectorScale( const vec_t* in, vec_t scale, vec_t* out );


#ifdef AngleVectors
#undef AngleVectors
#endif
void AngleVectors( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up );
void AngleVectorsTranspose( const vec_t* angles, vec_t* forward, vec_t* right, vec_t* up );
#define AngleIVectors	AngleVectorsTranspose

void AngleMatrix( const vec_t* angles, float(*matrix)[4] );
void AngleIMatrix( const vec_t* angles, float(*matrix)[4] );
void VectorTransform( const vec_t* in1, float(*in2)[4], vec_t* out );

void NormalizeAngles( float* angles );
void InterpolateAngles( float* start, float* end, float* output, float frac );
float AngleBetweenVectors( const vec3_t v1, const vec3_t v2 );


void VectorMatrix( vec_t* forward, vec_t* right, vec_t* up);
void VectorAngles( const vec_t* forward, vec_t* angles );

int InvertMatrix( const float * m, float *out );
float	anglemod(float a);

#ifdef __cplusplus
// Vector versions of the helpers above, preferred by C++ callers passing a Vector
class Vector;

void VectorAngles( const Vector &forward, Vector &angles );
float VectorLength( const Vector &v );
float VectorDistance( const Vector &v1, const Vector &v2 );
int VectorCompare( const Vector &v1, const Vector &v2 );
void CrossProduct( const Vector &v1, const Vector &v2, Vector &cross );
float VectorNormalize( Vector &v );
void VectorInverse( Vector &v );
void VectorInverseTemp( float *v );
void VectorScale( const Vector &in, float scale, Vector &out );
void NormalizeAngles( Vector &angles );
void InterpolateAngles( const Vector &start, const Vector &end, Vector &output, float frac );
float AngleBetweenVectors( const Vector &v1, const Vector &v2 );
void VectorMatrix( Vector &forward, Vector &right, Vector &up );
void VectorRotate( const Vector &in, const float matrix[3][4], Vector &out );
void VectorIRotate( const Vector &in, const float matrix[3][4], Vector &out );
void VectorScale( const Vector &in, float scale, float (&out)[4] );
void VectorInverse( float (&v)[4] );
#endif
