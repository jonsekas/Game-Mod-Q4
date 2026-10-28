#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

static const float ENCORE_DELAY_SCALE = 0.9f;

float idPlayer::GetSwingScale( void ) const
{
	float scaler = 1.0f;
	if ( HasSkill(SKILL_ENCORE) ) {
		scaler = ENCORE_DELAY_SCALE;
	}
	return scaler;
}

bool idPlayer::CanBlockBullets(void) const
{
	if ( HasSkill( SKILL_IMPOSSIBLYSKILLED) && weapon && weapon->isDual ) {
		return true;
	}
	return false;
}

bool idPlayer::TryReflect(void)
{
	if (!HasSkill(SKILL_IMPOSSIBLYSKILLED)) {
		return false;
	}
	if (gameLocal.time < parryEndTime) {
		return true;
	}
	int					elapsed;
	elapsed = gameLocal.time - blockStartTime;
	if ( pfl.isBlocking && weapon && elapsed <= weapon->perfectBlockWindow && gameLocal.time >= nextPerfectBlockTime && CanPerfectBlock()) {
		RewardPerfectBlock();
		return true;
	}
	return false;
}
