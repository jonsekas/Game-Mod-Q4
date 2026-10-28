#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

const float SPEED_SKILL_BONUS = 0.05f;
const float DEXTERITY_RELOAD_SCALE = 0.7f;

float idPlayer::GetMoveSpeedScale(void) const {
	float scale = 1.0f;
	if ( HasSkill( SKILL_UNBURDENED ) ) {
		scale += SPEED_SKILL_BONUS;
	}
	if ( HasSkill( SKILL_NIMBLE ) ) {
		scale += SPEED_SKILL_BONUS;
	}
	if ( HasSkill( SKILL_SWIFT ) ) {
		scale += SPEED_SKILL_BONUS;
	}
	return scale;
}

bool idPlayer::CanEvade( void ) const
{
	return HasSkill( SKILL_EVADE );
}

float idPlayer::GetReloadTimeScale( void ) const
{
	if (HasSkill( SKILL_DEXTERITY ) ) {
		return DEXTERITY_RELOAD_SCALE;
	}
	return 1.0f;
}

