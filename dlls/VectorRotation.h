#ifndef VECTORROTATION_H
#define VECTORROTATION_H

class Vector;

void RotateVectorX(Vector &vector, float radians);
void RotateVectorY(Vector &vector, float radians);
void RotateVectorZ(Vector &vector, float radians);
void UTIL_RotateVector(Vector *vector, const Vector &angles, const Vector &offset, bool reverse);
void UTIL_RotateVectorReverse(Vector *vector, const Vector &angles, const Vector &offset);

#endif
