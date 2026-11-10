#ifndef ASVECTOR_H
#define ASVECTOR_H

class CASDocumentation;
class Vector;
class Vector2D;

void ConstructVector2D(void *memory);
void CopyConstructVector2D(const Vector2D &vector, void *memory);
void Float2ConstructVector2D(float x, float y, void *memory);
void DestructVector2D(Vector2D *vector);
void ConstructVector3D(void *memory);
void CopyConstructVector3D(const Vector &vector, void *memory);
void Float3ConstructVector3D(float x, float y, float z, void *memory);
void DestructVector3D(Vector *vector);

void RegisterSCScriptVector2D(CASDocumentation *documentation);
void RegisterSCScriptVector3D(CASDocumentation *documentation);

#endif
