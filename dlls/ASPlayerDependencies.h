#ifndef ASPLAYERDEPENDENCIES_H
#define ASPLAYERDEPENDENCIES_H

class CASDocumentation;

// Observer methods require the player observer implementation
void RegisterSCScriptPlayerEntityDependencies( CASDocumentation *documentation );
void RegisterASPlayerAmmoMethods( CASDocumentation *documentation );
void RegisterASPlayerHUDMethods( CASDocumentation *documentation );

#endif
