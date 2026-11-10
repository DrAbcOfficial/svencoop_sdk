#ifndef CPLAYERMOVE_H
#define CPLAYERMOVE_H

// Needs Vector (vec3_t), physent_t, pmtrace_t and playermove_t (pm_defs.h) first

#ifndef MAX_CLIENTS
#define MAX_CLIENTS 32
#endif

class CPlayerMove
{
public:
	void ShowClipBox( void );
	void ParticleLine( const Vector &start, const Vector &end, int pcolor, float life, float vert );
	void DrawRectangle( const Vector &tl, const Vector &bl, const Vector &tr, const Vector &br, int pcolor, float life );
	void DrawPhysEntBBox( int num, int pcolor, float life );
	void DrawBBox( vec3_t mins, vec3_t maxs, vec3_t origin, int pcolor, float life );
	void ViewEntity( void );

	int GetVisEntInfo( int ent );
	int GetPhysEntInfo( int ent );
	playermove_t *GetCurrentPMove( void );

	void ScaleInput( float &forwardmove, float &sidemove, float &upmove );
	void ReduceTimers( void );
	qboolean AddToTouched( pmtrace_t tr, vec3_t impactvelocity );
	void CheckVelocity( void );
	int ClipVelocity( const Vector &in, const Vector &normal, Vector &out, float overbounce );
	void AddGravity( void );
	void StartGravity( void );
	void FinishGravity( void );
	void Accelerate( vec3_t wishdir, float wishspeed, float accel );
	void AirAccelerate( vec3_t wishdir, float wishspeed, float accel );
	void Friction( void );
	float SplineFraction( float value, float scale );
	pmtrace_t PushEntity( vec3_t push );
	float CalcRoll( vec3_t angles, vec3_t velocity, float rollangle, float rollspeed );
	void DropPunchAngle( Vector &punchangle );

	int FlyMove( void );
	void AirMove( void );
	void WalkMove( void );
	void WaterMove( void );
	void LadderMove( void );
	physent_t *CheckLadder( void );
	void NoClip( void );
	qboolean InWater( void );
	qboolean CheckWater( void );
	void CategorizePosition( void );
	void Physics_Toss( void );
	void CheckWaterJump( void );
	void WaterJump( void );
	void PlayWaterSounds( void );
	void UnDuck( void );
	void CheckParameters( void );

	void CreateStuckTable( void );
	void Init( playermove_t *ppmove );
	void FixPlayerCrouchStuck( int direction );
	void Duck( void );
	int GetRandomStuckOffsets( int nIndex, int server, Vector &offset );
	void ResetStuckOffsets( int nIndex, int server );
	int CheckStuck( void );

	int MapTextureTypeStepType( char chTextureType );
	void CatagorizeTextureType( void );
	int PlayerShouldBypassEntity( physent_t *entity );
	void GenericSplashSearch( void );
	void PlaySoundFX( int playerIndex, float *origin, int channel, char *sample, float volume, float attenuation, int flags, int pitch );
	void PlayStepSound( int step, float volume, bool force = false );
	void UpdateStepSound( bool force = false );

	void CheckFalling( void );
	void Jump( void );
	void FullWalkMove( void );
	void GetChaseOrigin( Vector angles, int target, Vector offset, Vector *origin );
	void SpectatorMove( void );
	void PlayerMove( bool server );
	void Move( playermove_t *ppmove, bool server );

private:
	bool			m_bInitialized;
	vec3_t			rgv3tStuckTable[54];
	int				rgStuckLast[MAX_CLIENTS][2];
	playermove_t	*pmove;
	physent_t		*m_pLadder;
	float			rgStuckCheckTime[MAX_CLIENTS][2];
	int				rgOverlappingEntity[MAX_CLIENTS];
};

extern CPlayerMove g_Pmove;

#endif
