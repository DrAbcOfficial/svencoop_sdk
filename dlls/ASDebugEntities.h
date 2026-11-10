#ifndef ASDEBUGENTITIES_H
#define ASDEBUGENTITIES_H

#include <string>

class CDebugger;

std::string SCScriptEntityToString(void *object, int expandMembersLevel, CDebugger *debugger);
std::string SCScriptPlayerEntityToString(void *object, int expandMembersLevel, CDebugger *debugger);

#endif
