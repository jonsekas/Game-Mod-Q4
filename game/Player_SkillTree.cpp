#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"


void idPlayer::OpenSkillTree(void)
{
	skillTreeOpen = true;
	UpdateSkillTreeGui();
	gameLocal.Printf("skill tree requested\n");
	if (skillTreeGui) {
		skillTreeGui->Activate( true, gameLocal.time);
		skillTreeGui->SetCursor(320, 240);

	}
}

void idPlayer::CloseSkillTree(void)
{
	skillTreeOpen = false;
	if (skillTreeGui) {
		skillTreeGui->Activate(false, gameLocal.time);
	}

}

void idPlayer::ToggleSkillTree(void)
{
	if (skillTreeOpen) {
		CloseSkillTree();
	} else {
		OpenSkillTree();
	}
}

bool idPlayer::DrawSkillTree(void)
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

void idPlayer::UpdateSkillTreeGui(void) 
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

bool idPlayer::UnlockSkill(skillId_t skill) 
{
	if (!skills.Unlock(skill)) {
		return false;
	}
	ApplySkillEffects();
	UpdateSkillTreeGui();
	return true;
}

void idPlayer::RefundSkills(void)
{
	skills.Refund();
	ApplySkillEffects();
	UpdateSkillTreeGui();
}

void idPlayer::GainXP(int amount)
{
	skills.GainXP( amount );
	UpdateSkillTreeGui();	
}

bool idPlayer::HasSkill(skillId_t skill) const
{
	return skills.Has( skill );
}

void idPlayer::ApplySkillEffects(void)
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