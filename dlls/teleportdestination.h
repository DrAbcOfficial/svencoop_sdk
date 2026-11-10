#ifndef TELEPORTDESTINATION_H
#define TELEPORTDESTINATION_H

#define SF_TELEPORTDEST_FIRE_ON_ARRIVAL 32

#define SF_TELEPORT_RANDOM_DESTINATION 64
#define SF_TELEPORT_RELATIVE 128
#define SF_TELEPORT_KEEP_ANGLES 256
#define SF_TELEPORT_KEEP_VELOCITY 512
#define SF_TELEPORT_ROTATE 1024
#define SF_TELEPORT_START_OFF 4096
#define SF_TELEPORT_IGNORE_DELAYS 8192
#define SF_TELEPORT_CHECK_BLOCKED 16384

class CTeleportDestination : public CPointEntity
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *data );

	float	m_flNextTeleportTime;
	float	m_flTeleportDelay;
	bool	m_bHasTeleportDelay;
};

#endif // TELEPORTDESTINATION_H
