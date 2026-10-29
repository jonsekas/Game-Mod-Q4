#include "../../idlib/precompiled.h"
#pragma hdrstop

#include "../Game_local.h"
#include "../Weapon.h"

class rvWeaponSword : public rvWeapon {
public:

	CLASS_PROTOTYPE(rvWeaponSword);

	rvWeaponSword(void);
	~rvWeaponSword(void);

	virtual void		Spawn(void);
	virtual void		CleanupWeapon(void);
	virtual void		Think(void);
	void				Save(idSaveGame* savefile) const;
	void				Restore(idRestoreGame* savefile);
	void				PreSave(void);
	void				PostSave(void);

protected:

	idAngles			bladeSpinFast;
	idAngles			bladeSpinSlow;
	jointHandle_t		bladeJoint;
	jointHandle_t		bladeJoint_world;
	int					bladeAccel;

	float				range;
	bool				single_swing;
	bool				waitForRelease;
	int					swingCost;
	int					nextSwingTime;
	int					swingDelay;
	idEntityPtr<idAnimatedEntity> offhandModel;
	idVec3				offhandOffset;
	int					blockDelay;

	rvClientEffectPtr	impactEffect;
	int					impactMaterial;

	void				Attack(void);
	void				Block(void);
	void				StartBlade(void);
	void				StopBlade(void);
	void				RemoveOffhand(void);

private:

	void				PlayLoopSound(int sndType);
	int					loopSound;
	enum {
		LOOP_NONE,
		LOOP_WALL,
		LOOP_FLESH
	};

	stateResult_t		State_Raise(const stateParms_t& parms);
	stateResult_t		State_Lower(const stateParms_t& parms);
	stateResult_t		State_Idle(const stateParms_t& parms);
	stateResult_t		State_Fire(const stateParms_t& parms);
	stateResult_t		State_Block(const stateParms_t& parms);

	CLASS_STATES_PROTOTYPE(rvWeaponSword);
};

CLASS_DECLARATION(rvWeapon, rvWeaponSword)
END_CLASS

/*
================
rvWeaponSword::rvWeaponSword
================
*/
rvWeaponSword::rvWeaponSword(void) {
	loopSound = LOOP_NONE;
}

/*
================
rvWeaponSword::~rvWeaponSword
================
*/
rvWeaponSword::~rvWeaponSword(void)
{
	if (viewModel) {
		StopSound(SND_CHANNEL_WEAPON, false);
	}
	if (impactEffect) {
		impactEffect->Stop();
		impactEffect = NULL;
	}
	impactMaterial = -1;
	RemoveOffhand();

}
/*
================
rvWeaponSword::Spawn
================
*/
void rvWeaponSword::Spawn(void) {
	SetState("Raise", 0);

	bladeJoint = viewModel->GetAnimator()->GetJointHandle(spawnArgs.GetString("joint_blade", "center"));
	bladeJoint_world = GetWorldModel()->GetAnimator()->GetJointHandle(spawnArgs.GetString("joint_blade", "center"));

	bladeSpinFast = spawnArgs.GetAngles("blade_spinfast");
	bladeSpinSlow = spawnArgs.GetAngles("blade_spinslow");
	bladeAccel = SEC2MS(spawnArgs.GetFloat("blade_accel", ".25"));

	range = spawnArgs.GetFloat("range", "32");
	single_swing = spawnArgs.GetBool("single_swing", "0");
	swingCost = spawnArgs.GetInt("swingCost", "0");
	nextSwingTime = 0;
	swingDelay = SEC2MS(spawnArgs.GetFloat("swingDelay", "0"));
	blockDelay = SEC2MS(spawnArgs.GetFloat("blockDelay", "0"));


	impactMaterial = -1;
	impactEffect = NULL;
	loopSound = LOOP_NONE;
	waitForRelease = false;

	offhandOffset = spawnArgs.GetVector("offhandOffset", "0 12 0");
	offhandModel = NULL;

	if (isDual) {
		idDict args;
		args.Set("classname", "player_animatedentity");

		idEntity* ent = NULL;
		gameLocal.SpawnEntityDef(args, &ent);

		if (ent) {
			idAnimatedEntity* model = static_cast<idAnimatedEntity*>(ent);
			model->SetModel( spawnArgs.GetString("model_view") );
			model->GetAnimator()->CycleAnim(ANIMCHANNEL_ALL, model->GetAnimator()->GetAnim("idle"), gameLocal.time, 0);
			offhandModel = model;
		}
	}
}

void rvWeaponSword::Think(void)
{
	rvWeapon::Think();

	idAnimatedEntity* offhand = offhandModel.GetEntity();
	if ( !offhand) {
		return;
	}
	offhand->SetOrigin( viewModelOrigin + offhandOffset * viewModelAxis );
	offhand->SetAxis( viewModelAxis );

	renderEntity_t* re = offhand->GetRenderEntity();
	re->allowSurfaceInViewID = owner->entityNumber + 1;
	re->weaponDepthHackInViewID = owner->entityNumber + 1;

	offhand->UpdateVisuals();
}

void rvWeaponSword::RemoveOffhand(void)
{
	idAnimatedEntity* offhand = offhandModel.GetEntity();
	if (offhand) {
		offhand->PostEventMS(&EV_Remove, 0);
	}
	offhandModel = NULL;
}
/*
================
rvWeaponSword::Save
================
*/
void rvWeaponSword::Save(idSaveGame* savefile) const {
	savefile->WriteAngles(bladeSpinFast);
	savefile->WriteAngles(bladeSpinSlow);
	savefile->WriteJoint(bladeJoint);
	savefile->WriteJoint(bladeJoint_world);

	savefile->WriteInt(bladeAccel);

	savefile->WriteFloat(range);
	savefile->WriteBool(single_swing);
	savefile->WriteBool(waitForRelease);
	savefile->WriteInt(swingCost);
	savefile->WriteInt(nextSwingTime);
	savefile->WriteInt(swingDelay);
	savefile->WriteInt(blockDelay);

	savefile->WriteObject(impactEffect);
	savefile->WriteInt(impactMaterial);
	savefile->WriteInt(loopSound);
}

/*
================
rvWeaponSword::Restore
================
*/
void rvWeaponSword::Restore(idRestoreGame* savefile) {
	savefile->ReadAngles(bladeSpinFast);
	savefile->ReadAngles(bladeSpinSlow);
	savefile->ReadJoint(bladeJoint);
	savefile->ReadJoint(bladeJoint_world);

	savefile->ReadInt(bladeAccel);

	savefile->ReadFloat(range);
	savefile->ReadBool(single_swing);
	savefile->ReadBool(waitForRelease);
	savefile->ReadInt(swingCost);
	savefile->ReadInt(nextSwingTime);
	savefile->ReadInt(swingDelay);
	savefile->ReadInt(blockDelay);

	savefile->ReadObject(reinterpret_cast<idClass*&>(impactEffect));
	savefile->ReadInt(impactMaterial);
	savefile->ReadInt(loopSound);
}

/*
================
rvWeaponSword::PreSave
================
*/
void rvWeaponSword::PreSave(void) {

}

/*
================
rvWeaponSword::PostSave
================
*/
void rvWeaponSword::PostSave(void) {
}

void rvWeaponSword::PlayLoopSound(int sndType) {
	if (loopSound == sndType) {
		return;
	}
	const char* loopSoundString = NULL;
	switch (sndType) {
	case LOOP_NONE:
	default:
		loopSoundString = "snd_spin_loop";
		break;
	case LOOP_WALL:
		loopSoundString = "snd_spin_wall";
		break;
	case LOOP_FLESH:
		loopSoundString = "snd_spin_flesh";
		break;
	}
	if (loopSoundString) {
		loopSound = sndType;
		StartSound(loopSoundString, SND_CHANNEL_WEAPON, 0, false, 0);
	}
}

/*
================
rvWeaponSword::CleanupWeapon
================
*/
void rvWeaponSword::CleanupWeapon(void) {

	if (impactEffect) {
		impactEffect->Stop();
		impactEffect = NULL;
	}
	impactMaterial = -1;
	PlayLoopSound(LOOP_NONE);
	RemoveOffhand();
}

/*
================
rvWeaponSword::Attack
================
*/
void rvWeaponSword::Attack(void) {
		float bonus = owner->ConsumeAttackBonus();
	trace_t		tr;
	idEntity* ent;

	// Cast a ray out to the lock range
// RAVEN BEGIN
// ddynerman: multiple clip worlds
	gameLocal.TracePoint(owner, tr,
		playerViewOrigin,
		playerViewOrigin + playerViewAxis[0] * range,
		MASK_SHOT_RENDERMODEL, owner);
	// RAVEN END
	owner->WeaponFireFeedback(&weaponDef->dict);

	if (tr.fraction >= 1.0f) {
		if (impactEffect) {
			impactEffect->Stop();
			impactEffect = NULL;
		}
		impactMaterial = -1;
		PlayLoopSound(LOOP_NONE);
		return;
	}

	// Entity we hit?
	ent = gameLocal.entities[tr.c.entityNum];

	// If the impact material changed then stop the impact effect 
	if ((tr.c.materialType && tr.c.materialType->Index() != impactMaterial) ||
		(!tr.c.materialType && impactMaterial != -1)) {
		if (impactEffect) {
			impactEffect->Stop();
			impactEffect = NULL;
		}
		impactMaterial = -1;
	}

	// In singleplayer-- the gauntlet never effects marine AI
	if (!gameLocal.isMultiplayer) {
		idActor* actor_ent = 0;

		//ignore both the body and the head.
		if (ent->IsType(idActor::GetClassType())) {
			actor_ent = static_cast<idActor*>(ent);
		}
		else if (ent->IsType(idAFAttachment::GetClassType())) {
			actor_ent = static_cast<idActor*>(ent->GetBindMaster());
		}

		if (actor_ent && actor_ent->team == gameLocal.GetLocalPlayer()->team) {
			PlayLoopSound(LOOP_NONE);
			return;
		}
	}

	//multiplayer-- don't gauntlet dead stuff
	if (gameLocal.isMultiplayer) {
		idPlayer* player;
		if (ent->IsType(idPlayer::GetClassType())) {
			player = static_cast<idPlayer*>(ent);
			if (player->health <= 0) {
				return;
			}
		}

	}

	if (!impactEffect) {
		impactMaterial = tr.c.materialType ? tr.c.materialType->Index() : -1;
		impactEffect = gameLocal.PlayEffect(gameLocal.GetEffect(spawnArgs, "fx_impact", tr.c.materialType), tr.endpos, tr.c.normal.ToMat3(), true);
	}
	else {
		impactEffect->SetOrigin(tr.endpos);
		impactEffect->SetAxis(tr.c.normal.ToMat3());
	}

	// Do damage?
	if (gameLocal.time > nextAttackTime) {
		if (ent) {
			if (ent->fl.takedamage) {
				float dmgScale = 1.0f;
				dmgScale *= owner->PowerUpModifier(PMOD_MELEE_DAMAGE);
				dmgScale *= bonus;
				gameLocal.Printf("hit x%.1f\n", dmgScale);
				ent->Damage(owner, owner, playerViewAxis[0], spawnArgs.GetString("def_damage"), dmgScale, CLIPMODEL_ID_TO_JOINT_HANDLE( tr.c.id ));
				StartSound("snd_hit", SND_CHANNEL_ANY, 0, false, NULL);
				if (ent->spawnArgs.GetBool("bleed")) {
					PlayLoopSound(LOOP_FLESH);
				}
				else {
					PlayLoopSound(LOOP_WALL);
				}
			}
			else {
				PlayLoopSound(LOOP_WALL);
			}
		}
		else {
			PlayLoopSound(LOOP_NONE);
		}
		nextAttackTime = gameLocal.time + fireRate;
	}
}

/*
================
rvWeaponSword::StartBlade
================
*/
void rvWeaponSword::StartBlade(void) {
	if (viewModel && bladeJoint != INVALID_JOINT ) {
		viewModel->GetAnimator()->SetJointAngularVelocity(bladeJoint, bladeSpinFast, gameLocal.time, bladeAccel);
	}

	if (GetWorldModel() && bladeJoint_world != INVALID_JOINT) {
		GetWorldModel()->GetAnimator()->SetJointAngularVelocity(bladeJoint_world, bladeSpinFast, gameLocal.time, bladeAccel);
	}

	StopSound(SND_CHANNEL_ITEM, false);
	//	StartSound ( "snd_blade_fast", SND_CHANNEL_ITEM, 0, false, NULL );
	StartSound("snd_spin_up", SND_CHANNEL_ITEM, 0, false, 0);
}

/*
================
rvWeaponSword::StopBlade
================
*/
void rvWeaponSword::StopBlade(void) {
	if (viewModel && bladeJoint != INVALID_JOINT ) {
		viewModel->GetAnimator()->SetJointAngularVelocity(bladeJoint, bladeSpinSlow, gameLocal.time, bladeAccel);
	}

	if (GetWorldModel() && bladeJoint_world != INVALID_JOINT ) {
		GetWorldModel()->GetAnimator()->SetJointAngularVelocity(bladeJoint_world, bladeSpinSlow, gameLocal.time, bladeAccel);
	}

	StopSound(SND_CHANNEL_WEAPON, false);
	//	StartSound ( "snd_blade_slow", SND_CHANNEL_ITEM, 0, false, NULL );

	if (impactEffect) {
		impactEffect->Stop();
		impactEffect = NULL;
	}
	impactMaterial = -1;
}

/*
===============================================================================

	States

===============================================================================
*/

CLASS_STATES_DECLARATION(rvWeaponSword)
STATE("Raise", rvWeaponSword::State_Raise)
STATE("Lower", rvWeaponSword::State_Lower)
STATE("Idle", rvWeaponSword::State_Idle)
STATE("Fire", rvWeaponSword::State_Fire)
STATE("Block", rvWeaponSword::State_Block)
END_CLASS_STATES

/*
================
rvWeaponSword::State_Raise
================
*/
stateResult_t rvWeaponSword::State_Raise(const stateParms_t& parms) {
	enum {
		RAISE_INIT,
		RAISE_WAIT,
	};
	switch (parms.stage) {
	case RAISE_INIT:
		SetStatus(WP_RISING);
		PlayAnim(ANIMCHANNEL_ALL, "raise", parms.blendFrames);
		return SRESULT_STAGE(RAISE_WAIT);

	case RAISE_WAIT:
		if (AnimDone(ANIMCHANNEL_ALL, 4)) {
			SetState("Idle", 4);
			return SRESULT_DONE;
		}
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponSword::State_Lower
================
*/
stateResult_t rvWeaponSword::State_Lower(const stateParms_t& parms) {
	enum {
		LOWER_INIT,
		LOWER_WAIT,
		LOWER_WAITRAISE
	};
	switch (parms.stage) {
	case LOWER_INIT:
		SetStatus(WP_LOWERING);
		PlayAnim(ANIMCHANNEL_ALL, "lower", parms.blendFrames);
		return SRESULT_STAGE(LOWER_WAIT);

	case LOWER_WAIT:
		if (AnimDone(ANIMCHANNEL_ALL, 0)) {
			SetStatus(WP_HOLSTERED);
			return SRESULT_STAGE(LOWER_WAITRAISE);
		}
		return SRESULT_WAIT;

	case LOWER_WAITRAISE:
		if (wsfl.raiseWeapon) {
			SetState("Raise", 0);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponSword::State_Idle
================
*/
stateResult_t rvWeaponSword::State_Idle(const stateParms_t& parms) {
	enum {
		IDLE_INIT,
		IDLE_WAIT,
	};
	switch (parms.stage) {
	case IDLE_INIT:
		SetStatus(WP_READY);
		StopBlade();
		PlayCycle(ANIMCHANNEL_ALL, "idle", parms.blendFrames);
		return SRESULT_STAGE(IDLE_WAIT);

	case IDLE_WAIT:
		if (wsfl.lowerWeapon) {
			SetState("Lower", 4);
			return SRESULT_DONE;
		}
		if (!wsfl.attack) {
			waitForRelease = false;

		}
		if (wsfl.attack && !waitForRelease && owner->inventory.stamina >= swingCost && gameLocal.time >= nextSwingTime) {
			SetState("Fire", 2);
			return SRESULT_DONE;
		}
		if ( wsfl.altAttack && gameLocal.time >= owner->nextBlockTime ) {
			SetState("Block", 2);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponSword::State_Fire
================
*/
stateResult_t rvWeaponSword::State_Fire(const stateParms_t& parms) {
	enum {
		STAGE_START,
		STAGE_START_WAIT,
		STAGE_LOOP,
		STAGE_LOOP_WAIT,
		STAGE_END,
		STAGE_END_WAIT
	};
	switch (parms.stage) {
	case STAGE_START:
		if (single_swing == true) {
			Attack();
<<<<<<< Updated upstream
			nextSwingTime = gameLocal.time + swingDelay;
=======
			StartSwingMotion();
			nextSwingTime = gameLocal.time + (int)( swingDelay * owner->GetSwingScale() );
>>>>>>> Stashed changes
			owner->StaminaManager(swingCost);
			waitForRelease = true;
			return SRESULT_STAGE(STAGE_END);
		}
		PlayAnim(ANIMCHANNEL_ALL, "attack_start", parms.blendFrames);
		StartBlade();
		loopSound = LOOP_NONE;
		return SRESULT_STAGE(STAGE_START_WAIT);

	case STAGE_START_WAIT:
		if (!wsfl.attack) {
			return SRESULT_STAGE(STAGE_END);
		}
		if (AnimDone(ANIMCHANNEL_ALL, parms.blendFrames)) {
			return SRESULT_STAGE(STAGE_LOOP);
		}
		return SRESULT_WAIT;

	case STAGE_LOOP:
		PlayCycle(ANIMCHANNEL_ALL, "attack_loop", parms.blendFrames);
		StartSound("snd_spin_loop", SND_CHANNEL_WEAPON, 0, false, 0);
		return SRESULT_STAGE(STAGE_LOOP_WAIT);

	case STAGE_LOOP_WAIT:
		if (!wsfl.attack || wsfl.lowerWeapon) {
			return SRESULT_STAGE(STAGE_END);
		}
		Attack();
		return SRESULT_WAIT;

	case STAGE_END:
		PlayAnim(ANIMCHANNEL_ALL, "attack_end", parms.blendFrames);
		StopBlade();
		StartSound("snd_spin_down", SND_CHANNEL_WEAPON, 0, false, 0);
		return SRESULT_STAGE(STAGE_END_WAIT);

	case STAGE_END_WAIT:
		if (wsfl.attack || AnimDone(ANIMCHANNEL_ALL, parms.blendFrames)) {
			PostState("Idle", parms.blendFrames);
			return SRESULT_DONE;
		}
		return SRESULT_WAIT;
	}
	return SRESULT_ERROR;
}

/*
================
rvWeaponSword :: State_Block
================
*/
stateResult_t rvWeaponSword::State_Block(const stateParms_t& parms) {
	enum {
		STAGE_START,
		STAGE_WAIT,
		STAGE_END
	};
	switch (parms.stage) {

	case STAGE_START:
		owner->pfl.isBlocking = true;
		owner->blockStartTime = gameLocal.time;
		return SRESULT_STAGE(STAGE_WAIT);
	case STAGE_WAIT:
		if (!wsfl.altAttack || wsfl.lowerWeapon) {
			return SRESULT_STAGE(STAGE_END);
		}
		return SRESULT_WAIT;
	case STAGE_END:
		owner->pfl.isBlocking = false;
		owner->nextBlockTime = gameLocal.time + owner->GetGuardCooldown( blockCooldown );
		PostState("Idle", parms.blendFrames);
		return SRESULT_DONE;
	}
	return SRESULT_ERROR;
}




