#ifndef HGRUNT_REPEL_H
#define HGRUNT_REPEL_H

void HumanGruntRepelPrecache( CBaseMonster *owner, const char *className,
                              const char *model, bool invertAlly, int &sprite );
void HumanGruntRepelUse( CBaseMonster *owner, const char *className, int head = 0 );

#endif
