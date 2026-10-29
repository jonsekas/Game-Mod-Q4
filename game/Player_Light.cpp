#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

static const float BLESSED_POWDER_DAMAGE_BONUS = 0.25f;
static const float THE_MANIFESTO_DAMAGE_BONUS = 0.25f;
static const float THE_LIGHT_DAMAGE_BONUS = 0.3f;

float idPlayer::GetFireArmDamageScale(void) const
{
	float scale = 1.0f;
	if ( HasSkill( SKILL_BLESSEDPOWDER ) ) {
		scale += BLESSED_POWDER_DAMAGE_BONUS;
	}
	if ( HasSkill( SKILL_THEMANIFESTO ) ) {
		scale += THE_MANIFESTO_DAMAGE_BONUS;
	}
	return scale;
}

float idPlayer::GetThrowableDamageScale(void) const
{
	float damageScale = 1.0f;
	if ( HasSkill( SKILL_THELIGHT) ) {
		damageScale += THE_LIGHT_DAMAGE_BONUS;
	}
	return damageScale;
}