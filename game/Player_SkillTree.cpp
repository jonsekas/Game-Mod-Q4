#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

static const float PALADIN_XP_BONUS = 0.1f;
static const float HERETIC_XP_BONUS = 0.05f;

void idPlayer::OpenSkillTree( void )
{
	skillTreeOpen = true;
	UpdateSkillTreeGui();
	gameLocal.Printf("skill tree requested\n");
	if (skillTreeGui) {
		skillTreeGui->Activate( true, gameLocal.time);
		skillTreeGui->SetCursor(320, 240);

	}
}

void idPlayer::CloseSkillTree( void )
{
	skillTreeOpen = false;
	if (skillTreeGui) {
		skillTreeGui->Activate(false, gameLocal.time);
	}

}

void idPlayer::ToggleSkillTree( void )
{
	if (skillTreeOpen) {
		CloseSkillTree();
	} else {
		OpenSkillTree();
	}
}

bool idPlayer::DrawSkillTree( void )
{
	if (skillTreeGui && skillTreeOpen) {
		skillTreeGui->Redraw(gameLocal.time);
		skillTreeGui->DrawCursor();
		return true;
	}
	return false;
}

bool idPlayer::HandleSkillTreeCommand( const idToken &token, idLexer*src )
{
	if ( token.Icmp( "closeSkillTree") == 0) {
		CloseSkillTree();
		return true;
	}
	if ( token.Icmp("skillUnlock") == 0 ) {
		idToken skillName;
		if (!src->ReadToken(&skillName)) {
			return true;
		}
		skillId_t skill = skills.Find(skillName.c_str());
		gameLocal.Printf("skill clicked: %s -> %d\n", skillName.c_str(), skill);
		UnlockSkill( skill );
		return true;
	}
	if (token.Icmp("resetSkillTree") == 0) {
		RefundSkills();
		return true;
	}
	return false;

}

void idPlayer::UpdateSkillTreeGui( void ) 
{
	if (!skillTreeGui) {
		return;
	}
	for (int i = 0; i < NUM_SKILLS; i++) {
		skillId_t skill = (skillId_t)i;
		skillTreeGui->SetStateInt(va( "skill_%s", skills.GetName( skill ) ), skills.GetState( skill ) );
	}
	skillTreeGui->SetStateInt("skillPoints", skills.GetPoints());
	skillTreeGui->SetStateInt("level", skills.GetLevel());
	skillTreeGui->SetStateInt("xp", skills.GetXP());

	int current = skills.GetXP();
	int needed = skills.XpForNextLevel();
	skillTreeGui->SetStateFloat("xpPct", (float)current / (float)needed);
	skillTreeGui->SetStateString( "xpText", va("%d/%d", current, needed) );
	skillTreeGui->StateChanged(gameLocal.time);
}

bool idPlayer::UnlockSkill( skillId_t skill ) 
{
	if (!skills.Unlock(skill)) {
		return false;
	}
	const char* weapon = skills.GetWeapon(skill);
	if (weapon != NULL) {
		GiveItem(weapon);
	}
	ApplySkillEffects();
	UpdateSkillTreeGui();
	return true;
}

void idPlayer::RefundSkills( void )
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		skillId_t skill = (skillId_t)i;
		const char* weapon = skills.GetWeapon( skill );
		if ( HasSkill(skill)&& weapon != NULL ) {
			int slot = SlotForWeapon(weapon);
			if (slot >= 0) {
				inventory.weapons &= ~(1 << slot);
				if ( currentWeapon == slot || idealWeapon == slot ) {
					NextBestWeapon();
				}
			}
		}	
	}
	skills.Refund();
	ApplySkillEffects();
	UpdateSkillTreeGui();
}

void idPlayer::GainXP( int amount )
{
	int scaled = (int)( amount * GetXpScale() + 0.5f );
	skills.GainXP( scaled );
	UpdateSkillTreeGui();	
}

float idPlayer::GetXpScale(void) const
{
	float xpScale = 1.0f;
	if ( HasSkill( SKILL_PALADIN ) ) {
		xpScale += PALADIN_XP_BONUS;
	}
	if ( HasSkill( SKILL_HERETIC ) ) {
		xpScale += HERETIC_XP_BONUS;
	}
	return xpScale;
}

bool idPlayer::HasSkill( skillId_t skill ) const
{
	return skills.Has( skill );
}

void idPlayer::ApplySkillEffects( void )
{
	inventory.maxstamina = MAX_STAMINA + GetStaminaBonus();
	if (inventory.stamina > inventory.maxstamina) {
		inventory.stamina = inventory.maxstamina;
	}
	inventory.maxHealth = spawnArgs.GetInt("maxhealth", "100") + GetHealthBonus();
	if (health > inventory.maxHealth) {
		health = inventory.maxHealth;
	}
}

