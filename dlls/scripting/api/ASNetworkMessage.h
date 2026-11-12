#ifndef ASNETWORKMESSAGE_H
#define ASNETWORKMESSAGE_H

class CBaseEntity;
class CASDocumentation;
class CString;
class Vector;
struct edict_s;

class CASNetworkMessage
{
public:
	CASNetworkMessage(int destination, int type, edict_s *edict);
	CASNetworkMessage(int destination, int type, const Vector &origin, edict_s *edict);
	~CASNetworkMessage();

	void Release(void) const;
	void End(void);
	void WriteByte(unsigned char value);
	void WriteChar(const CString &text);
	void WriteShort(unsigned short value);
	void WriteLong(unsigned long value);
	void WriteAngle(float value);
	void WriteCoord(float value);
	void WriteVector(const Vector &value);
	void WriteString(const CString &text);
	void WriteEntity(int index);
	void WriteEntity(CBaseEntity *entity);
	void WriteFloat(float value);

private:
	static CASNetworkMessage *m_pLastCreatedMessage;

	bool m_bEnded;
};

void RegisterSCScriptNetworkMessageClass(CASDocumentation *documentation);
void RegisterSCScriptNetworkMessage(CASDocumentation *documentation);

#endif
