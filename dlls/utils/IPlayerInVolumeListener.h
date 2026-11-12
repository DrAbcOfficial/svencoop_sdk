#ifndef IPLAYERINVOLUMELISTENER_H
#define IPLAYERINVOLUMELISTENER_H

class CBasePlayer;

class IPlayerInVolumeListener
{
public:
	virtual ~IPlayerInVolumeListener() {}
	virtual void PlayerDetected( bool inVolume, CBasePlayer *player ) = 0;
};

#endif
