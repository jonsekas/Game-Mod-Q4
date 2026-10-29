#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"
#include "Projectile.h"

const int STAMINA_SKILL_BONUS = 25;
const int SIDESTEP_GAIN_BONUS = 10;
const float SIDESTEP_COOLDOWN_SCALE = 0.8f;
const float RIPOSTE_WINDOW_SCALE = 1.5f;
const float PARRY_DAMAGE_BONUS = 2.0f;
const float RIPOSTE_DAMAGE_BONUS = 2.5f;

int idPlayer::GetPerfectBlockGain(void) const {
	if ( !weapon ) {
		return 0;
	}
	int gain = weapon->perfectBlockGain;
	if (HasSkill(SKILL_SIDESTEP)) {
		gain += SIDESTEP_GAIN_BONUS;
	}
	return gain;
}

bool idPlayer::CanPerfectBlock(void) const
{
	return HasSkill( SKILL_BLOCK );
}

int idPlayer::GetStaminaBonus(void) const 
{
	int bonus = 0;
	if (HasSkill(SKILL_FIGHTER)) {
		bonus += STAMINA_SKILL_BONUS;
	}
	if (HasSkill( SKILL_WARRIOR) ) {
		bonus += STAMINA_SKILL_BONUS;
	}
	return bonus;
}

int idPlayer::GetGuardCooldown( int baseCooldown )
{
	if (!HasSkill(SKILL_SIDESTEP)) {
		return baseCooldown;
	}
	else if (pfl.skipBlockCooldown) {
		pfl.skipBlockCooldown = false;
		return 0;
	}
	else {
		return (int)(baseCooldown * SIDESTEP_COOLDOWN_SCALE);
	}
}

int idPlayer::GetParryWindow( void ) const
{
	if (!weapon)  {
		return 0;
	}
	int window = weapon->parryWindow;
	if (HasSkill(SKILL_RIPOSTE)) {
			window = (int)(window * RIPOSTE_WINDOW_SCALE);
	}
	return window;
}

void idPlayer::StartParry(void)
{
	if ( !HasSkill( SKILL_PARRY )) {
		return;
	}
	if (!weapon || !weapon->parryWindow ) {
		return;
	}
	if (gameLocal.time < nextParryTime) {
		return;
	}
	if (inventory.stamina < weapon->parryCost) {
		return;
	}	
	StaminaManager(weapon->parryCost);
	parryRefund = weapon->parryCost;
	parryEndTime = gameLocal.time + GetParryWindow();
	nextParryTime = gameLocal.time + weapon->parryCooldown;
	gameLocal.Printf("parry! window %d ms\n", GetParryWindow());
}

bool idPlayer::TryParry(idEntity* inflictor, idEntity* attacker)
{
	if ( pfl.hitByBullet ) {
		return false;
	}
	if ( inflictor && inflictor->IsType( idProjectile::GetClassType() ) ) {
		return false;
	}

	if (gameLocal.time > parryEndTime) {
		return false;
	}
	if (attacker == this || !attacker->IsType(idActor::GetClassType())) {
		return false;
	}
	inventory.stamina += parryRefund;
	parryRefund = 0;
	inventory.stamina = idMath::ClampInt(0, inventory.maxstamina, inventory.stamina);
	attackBonus = HasSkill(SKILL_RIPOSTE) ? RIPOSTE_DAMAGE_BONUS : PARRY_DAMAGE_BONUS;
	if (!HasSkill(SKILL_RIPOSTE)) {
		parryEndTime = 0;
	}
	gameLocal.Printf("PARRIED! next swing x%.1f\n", attackBonus);
	return true;

}

float idPlayer::ConsumeAttackBonus( void )
{
	float bonus = attackBonus;
	attackBonus = 1.0f;
	return bonus;
}