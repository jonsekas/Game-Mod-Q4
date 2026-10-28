#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "Weapon.h"

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