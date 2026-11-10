//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// Camera.h  --  defines and such for a 3rd person camera
// NOTE: must include quakedef.h first

#ifndef _CAMERA_H_
#define _CAMERA_H_

// pitch, yaw, dist
extern vec3_t cam_ofs;
// Using third person camera
extern int cam_thirdperson;
extern bool cam_thirdperson_clipped;
extern cvar_t *cam_idealdist;
extern cvar_t *cam_followaim;
extern vec3_t gAimVector;

void CAM_CalcAngles(float *camAngles, float *viewangles);
float MoveToward(float cur, float goal, float maxspeed);
void CAM_Init( void );
void CAM_ClearStates( void );
int CAM_KeyEvent( int down, int keynum );
void IN_CameraMouseLook( float *angles );
void IN_CameraAccumulate( void );

#endif		// _CAMERA_H_
