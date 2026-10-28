#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

static const float MARKSMAN_STILL_SCALE = 0.5f;

static const float MARKSMAN_CROUCH_SCALE = 0.25f;

float idPlayer::GetFireArmSpreadScale(void) const
{
	if ( !HasSkill( SKILL_MARKSMAN ) ) {
		return 1.0f;
	}
	if ( usercmd.forwardmove != 0 || usercmd.rightmove != 0 ) {
		return 1.0f;
	}

	if ( physicsObj.IsCrouching() ) {
		return MARKSMAN_CROUCH_SCALE;
	}
	return MARKSMAN_STILL_SCALE;
}