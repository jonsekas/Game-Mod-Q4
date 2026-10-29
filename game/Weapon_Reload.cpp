#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "Weapon.h"

static const float SWING_SWEEP_DELAY = 0.3f;

void rvWeapon::StartReloadTimer(void)
{
	float scale = owner ? owner->GetReloadTimeScale() : 1.0f;
	reloadDuration = (int)( reloadTime * scale );
	reloadStartTime = gameLocal.time;
	reloadEndTime = gameLocal.time + reloadDuration;
	reloadCancelArmed = false;
}

void rvWeapon::StopReloadTimer(void)
{
	reloadStartTime = 0;
	reloadEndTime = 0;
	reloadDuration = 0;
}

bool rvWeapon::ReloadTimerDone(void) const
{
	return gameLocal.time >= reloadEndTime;	
}

float rvWeapon::GetReloadProgress(void) const
{
	if ( reloadEndTime == 0 || reloadDuration <= 0) {
		return 0.0f;
	}
	return (float)(gameLocal.time - reloadStartTime) / (float)reloadDuration;
}

bool rvWeapon::ReloadCancelRequested(void)
{
	if (!wsfl.attack) {
		reloadCancelArmed = true;
		return false;
	}
	return reloadCancelArmed;
}

bool rvWeapon::IsReloadTimerRunning(void) const
{
	return reloadEndTime != 0;
}

void rvWeapon::SwingMotion(idVec3 &origin, idMat3 &axis)
{
	if (swingStartTime == 0) {
		return;
	}
	int elapsed = gameLocal.time - swingStartTime;
	if (elapsed >= swingTime) {
		swingStartTime = 0;
		return;
	}
	float frac = (float)elapsed / (float)swingTime;
	float lunge = idMath::Sin(frac * idMath::PI);          
	float sweepFrac = (frac - SWING_SWEEP_DELAY) / (1.0f - SWING_SWEEP_DELAY);
	sweepFrac = idMath::ClampFloat(0.0f, 1.0f, sweepFrac);                    
	float sweep = idMath::Sin(sweepFrac * idMath::PI);

	idAngles ang = swingAngles * sweep;
	idVec3   offset = swingOffset * lunge;
	origin = origin - axis * offset;
	axis = ang.ToMat3() * axis;
}

void rvWeapon::StartSwingMotion(void) {
	swingStartTime = gameLocal.time;
}