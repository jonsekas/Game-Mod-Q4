#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"


bool idPlayer::TryBlock( idEntity* attacker) {


	if ( !pfl.isBlocking ) {
		return false;
	}
	if ( attacker == this || !attacker->IsType(idActor::GetClassType()) ) {
		return false;
	}
	if (!weapon) {
		return false;
	}
	if (inventory.stamina < weapon->blockCost) {	
		return false;
	}
	int					elapsed;
	elapsed = gameLocal.time - blockStartTime;

	if ( elapsed <= weapon->perfectBlockWindow && gameLocal.time >= nextPerfectBlockTime && CanPerfectBlock() ) {
		inventory.stamina += GetPerfectBlockGain();
		inventory.stamina = idMath::ClampInt(0, inventory.maxstamina, inventory.stamina);
		nextPerfectBlockTime = gameLocal.time + weapon->perfectBlockCooldown;
		if (HasSkill(SKILL_SIDESTEP)) {
			pfl.skipBlockCooldown = true;
		}
	}
	else {
		StaminaManager( weapon->blockCost );
	}
	return true;


}

// 	// MCG: player doesn't take friendly fire damage, except from self!
// if (!gameLocal.isMultiplayer && attacker != this) {
	// if (attacker->IsType(idActor::GetClassType()) && static_cast<idActor*>(attacker)->team == team) {
		// return;
	// }
// }