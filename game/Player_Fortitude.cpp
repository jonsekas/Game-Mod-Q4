#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

const int HEALTH_SKILL_BONUS = 25;
const int UNBURDENED_EVADE_DISCOUNT = 15;
const float OUTLASTING_DRAIN_SCALE = 0.75f;

int idPlayer::GetHealthBonus( void ) const
{
	int bonus = 0;
	if ( HasSkill( SKILL_BRAVE ) )
	{
		bonus += HEALTH_SKILL_BONUS;
	}
	if ( HasSkill( SKILL_VIGOR ) )
	{
		bonus += HEALTH_SKILL_BONUS;
	}
	return bonus;
}

int idPlayer::GetEvadeCostReduction( void ) const
{
	if ( HasSkill( SKILL_UNBURDENED ) ) {
		return UNBURDENED_EVADE_DISCOUNT;
	}
	return 0;
}

float idPlayer::GetSprintDrainScale( void ) const
{
	if ( HasSkill( SKILL_OUTLASTING ) ) {
		return OUTLASTING_DRAIN_SCALE;
	}
	return 1.0f;
}

