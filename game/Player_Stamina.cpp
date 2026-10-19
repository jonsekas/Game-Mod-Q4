#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"


const int	STAMINA_PULSE = 100;			// Regen rate for Stamina System
const int	STAMINA_DRAIN = 50;			// Regen rate for Stamina System
const int	STAMINA_JUMP_COST = 50;			// Regen rate for Stamina System
const float   EVADE_SPEED = 400.0f;
const float   EVADE_HOP = 50.0f;
const int   EVADE_STAMINA_COST = 50;
const int   EVADE_COOLDOWN = 5000;


/*
==============
idPlayer:: StaminaSys
==============	
*/
void idPlayer::StaminaSys(void) {

	bool running = !physicsObj.OnLadder() && (usercmd.buttons & BUTTON_RUN) && (usercmd.forwardmove || usercmd.rightmove) && (usercmd.upmove >= 0);

	if (physicsObj.HasJumped()) {
		inventory.stamina -= STAMINA_JUMP_COST;
		gameLocal.Printf("JUMP! stamina now %d\n", inventory.stamina);
	}

	if (inventory.stamina < STAMINA_JUMP_COST && usercmd.upmove > 0) {
		usercmd.upmove = 0;
	}

	if (gameLocal.time >= nextStaminaPulse) {
		if (running && inventory.stamina > 0) {
			inventory.stamina--;
			nextStaminaPulse = gameLocal.time + STAMINA_DRAIN;
		}
		else if (inventory.stamina < inventory.maxstamina) {
			inventory.stamina++;
			nextStaminaPulse = gameLocal.time + STAMINA_PULSE;
		}
	}

	inventory.stamina = idMath::ClampInt(0, inventory.maxstamina, inventory.stamina);

}

/*
==============
idPlayer:: Evade
==============
*/

void idPlayer::Evade(void) {
	if (usercmd.forwardmove == 0 && usercmd.rightmove == 0) {
		return;
	}
	if (gameLocal.time < nextEvade || inventory.stamina < EVADE_STAMINA_COST) {
		return;
	}
	gameLocal.Printf("forward: %d  right: %d\n", usercmd.forwardmove, usercmd.rightmove);
	idVec3 vel = physicsObj.GetLinearVelocity();
	idVec3 forward;
	idVec3 right;

	viewAngles.ToVectors(&forward, &right, NULL);
	idVec3 dir;
	dir.Zero();
	dir += forward * usercmd.forwardmove;
	dir += right * usercmd.rightmove;
	gameLocal.Printf("vel: %f %f %f\n", vel.x, vel.y, vel.z);
	dir.z = 0.0f;
	dir.Normalize();
	vel += dir * EVADE_SPEED;
	vel.z += EVADE_HOP;
	physicsObj.SetLinearVelocity(vel);
	inventory.stamina -= EVADE_STAMINA_COST;
	nextEvade = gameLocal.time + EVADE_COOLDOWN;
}