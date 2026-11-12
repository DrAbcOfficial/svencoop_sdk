#include "scripting/api/ASBLOB.h"
#include "scripting/core/CASBLOB.h"
#include "scripting/core/CASDocumentation.h"

void RegisterSCScriptBLOB(CASDocumentation *documentation)
{
	documentation->RegisterObjectType("Binary Large OBject class used for storing arbitrary amounts of binary data.", "BLOB", 0, asOBJ_REF);
	documentation->RegisterObjectBehaviour("AddRef", "BLOB", asBEHAVE_ADDREF,
		"void AddRef()", asMETHOD(CASBLOB, AddRef), asCALL_THISCALL);
	documentation->RegisterObjectBehaviour("Release", "BLOB", asBEHAVE_RELEASE,
		"void Release()", asMETHOD(CASBLOB, Release), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether this instance has allocated a buffer.", "BLOB",
		"bool HasAllocatedBuffer() const",
		asMETHOD(CASBLOB, HasAllocatedBuffer), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the size of this BLOB, in bytes.", "BLOB",
		"size_t GetSizeInBytes() const",
		asMETHOD(CASBLOB, GetSizeInBytes), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the read offset, in bytes.", "BLOB",
		"size_t GetReadOffset() const",
		asMETHOD(CASBLOB, GetReadOffset), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the last read operation reached the end of data that was written to this BLOB.", "BLOB",
		"bool ReadReachedEnd() const",
		asMETHOD(CASBLOB, ReadReachedEnd), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the given amount of bytes can be read from this BLOB.", "BLOB",
		"bool ReadSizeValid(size_t uiSizeInBytes) const",
		asMETHOD(CASBLOB, ReadSizeValid), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the write offset, in bytes. This is the number of bytes written to this buffer.", "BLOB",
		"size_t GetWriteOffset() const",
		asMETHOD(CASBLOB, GetWriteOffset), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether the given amount of bytes can be written to this BLOB.", "BLOB",
		"bool CanWriteSize(size_t uiSizeInBytes) const",
		asMETHOD(CASBLOB, CanWriteSize), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Gets the number of bytes left that can be written to. This may change if a write operation causes the buffer to be resized.", "BLOB",
		"size_t GetWriteBytesLeft() const",
		asMETHOD(CASBLOB, GetWriteBytesLeft), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Returns whether this BLOB is allowed to resize its buffer.", "BLOB",
		"bool CanResize() const",
		asMETHOD(CASBLOB, CanResize), asCALL_THISCALL);

	// The one-argument declaration is bound to the no-argument getter,
	// despite the calling-convention mismatch
	documentation->RegisterObjectMethod("Sets whether this BLOB is allowed to resize its buffer.", "BLOB",
		"void SetCanResize(bool fState)",
		asMETHOD(CASBLOB, CanResize), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int8 from this BLOB.", "BLOB",
		"int8 ReadInt8()",
		asMETHODPR(CASBLOB, ReadInt8, (void), signed char), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int16 from this BLOB.", "BLOB",
		"int16 ReadInt16()",
		asMETHODPR(CASBLOB, ReadInt16, (void), short), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int32 from this BLOB.", "BLOB",
		"int32 ReadInt32()",
		asMETHODPR(CASBLOB, ReadInt32, (void), int), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int64 from this BLOB.", "BLOB",
		"int64 ReadInt64()",
		asMETHODPR(CASBLOB, ReadInt64, (void), long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint8 from this BLOB.", "BLOB",
		"uint8 ReadUInt8()",
		asMETHODPR(CASBLOB, ReadInt8, (void), signed char), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint16 from this BLOB.", "BLOB",
		"int16 ReadUInt16()",
		asMETHODPR(CASBLOB, ReadInt16, (void), short), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint32 from this BLOB.", "BLOB",
		"int32 ReadUInt32()",
		asMETHODPR(CASBLOB, ReadInt32, (void), int), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint64 from this BLOB.", "BLOB",
		"int64 ReadUInt64()",
		asMETHODPR(CASBLOB, ReadInt64, (void), long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a float from this BLOB.", "BLOB",
		"float ReadFloat()",
		asMETHODPR(CASBLOB, ReadFloat, (void), float), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a double from this BLOB.", "BLOB",
		"double ReadDouble()",
		asMETHODPR(CASBLOB, ReadDouble, (void), double), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int8 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int8 ReadInt8(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt8, (bool &), signed char), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int16 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int16 ReadInt16(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt16, (bool &), short), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int32 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int32 ReadInt32(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt32, (bool &), int), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads an int64 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int64 ReadInt64(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt64, (bool &), long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint8 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"uint8 ReadUInt8(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt8, (bool &), signed char), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint16 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int16 ReadUInt16(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt16, (bool &), short), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint32 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int32 ReadUInt32(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt32, (bool &), int), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a uint64 from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"int64 ReadUInt64(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadInt64, (bool &), long long), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a float from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"float ReadFloat(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadFloat, (bool &), float), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a double from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"double ReadDouble(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadDouble, (bool &), double), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a fixed length string from this BLOB.", "BLOB",
		"string ReadString(size_t uiSizeInBytes)",
		asMETHODPR(CASBLOB, ReadString, (unsigned int), CString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a fixed length string from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"string ReadString(size_t uiSizeInBytes, bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadString, (unsigned int, bool &), CString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a variable length string from this BLOB. fSuccess is true if the read succeeded.", "BLOB",
		"string ReadString(bool& out fSuccess)",
		asMETHODPR(CASBLOB, ReadString, (bool &), CString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reads a variable length string from this BLOB.", "BLOB",
		"string ReadString()",
		asMETHODPR(CASBLOB, ReadString, (void), CString), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a given value a number of bytes to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool WriteBytes(size_t uiSizeInBytes, int iValue = 0)",
		asMETHOD(CASBLOB, WriteBytes), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a given value until the given end offset to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool WriteBytesUntil(size_t uiEndOffset, int iValue = 0)",
		asMETHOD(CASBLOB, WriteBytesUntil), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an int8 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(int8 data)",
		asMETHODPR(CASBLOB, Write, (unsigned char), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an int16 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(int16 data)",
		asMETHODPR(CASBLOB, Write, (unsigned short), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an int32 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(int32 data)",
		asMETHODPR(CASBLOB, Write, (unsigned long), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes an int64 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(int64 data)",
		asMETHODPR(CASBLOB, Write, (unsigned long long), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a uint8 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(uint8 data)",
		asMETHODPR(CASBLOB, Write, (unsigned char), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a uint16 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(uint16 data)",
		asMETHODPR(CASBLOB, Write, (unsigned short), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a uint32 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(uint32 data)",
		asMETHODPR(CASBLOB, Write, (unsigned long), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a uint64 to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(uint64 data)",
		asMETHODPR(CASBLOB, Write, (unsigned long long), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a float to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(float data)",
		asMETHODPR(CASBLOB, Write, (float), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a double to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(double data)",
		asMETHODPR(CASBLOB, Write, (double), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a fixed length string to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(const string& in szString, size_t uiSizeInBytes)",
		asMETHODPR(CASBLOB, Write, (const CString &, unsigned int), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Writes a variable length string to this BLOB. Returns true if the write succeeded.", "BLOB",
		"bool Write(const string& in szString)",
		asMETHODPR(CASBLOB, Write, (const CString &), bool), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Resizes this BLOB. Data may be lost because of this.", "BLOB",
		"void Resize(size_t uiNewSize)",
		asMETHOD(CASBLOB, Resize), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Reserves the given amount of space, if needed.", "BLOB",
		"void Reserve(size_t uiMinimumSize)",
		asMETHOD(CASBLOB, Reserve), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Shrinks this BLOB to fit the data that was written to it.", "BLOB",
		"void ShrinkToFit()",
		asMETHOD(CASBLOB, ShrinkToFit), asCALL_THISCALL);
	documentation->RegisterObjectMethod("Clears this BLOB of all data and allocated memory. All flags are reset to their default values.", "BLOB",
		"void Clear()",
		asMETHOD(CASBLOB, Clear), asCALL_THISCALL);
}
