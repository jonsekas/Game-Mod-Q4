#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

static const skillDef_t skillTable[NUM_SKILLS] = {
	{ "Parry", SKILL_NONE, SKILLCAT_PROWESS },
	{ "Fighter", SKILL_NONE, SKILLCAT_PROWESS },
	{ "Block", SKILL_NONE, SKILLCAT_PROWESS },
	{ "Riposte", SKILL_PARRY, SKILLCAT_PROWESS },
	{ "Warrior", SKILL_FIGHTER, SKILLCAT_PROWESS },
	{ "Sidestep", SKILL_BLOCK, SKILLCAT_PROWESS },
	{ "Unburdened", SKILL_NONE, SKILLCAT_FORTITUDE },
	{ "Brave", SKILL_NONE, SKILLCAT_FORTITUDE },
	{ "Outlasting", SKILL_UNBURDENED, SKILLCAT_FORTITUDE },
	{ "Vigor", SKILL_BRAVE, SKILLCAT_FORTITUDE },
	{ "Greatsword", SKILL_NONE, SKILLCAT_LIGHT },
	{ "blessedPowder", SKILL_NONE, SKILLCAT_LIGHT },
	{ "Spear", SKILL_GREATSWORD, SKILLCAT_LIGHT },
	{ "theLight", SKILL_BLESSEDPOWDER, SKILLCAT_LIGHT },
	{ "Paladin", SKILL_SPEAR, SKILLCAT_LIGHT },
	{ "theManifesto", SKILL_THELIGHT, SKILLCAT_LIGHT },
	{ "Longsword", SKILL_NONE, SKILLCAT_DARK },
	{ "dualDaggers", SKILL_NONE, SKILLCAT_DARK },
	{ "Heretic", SKILL_LONGSWORD, SKILLCAT_DARK },
	{ "Encore", SKILL_DUALDAGGERS, SKILLCAT_DARK },
	{ "impossiblySkilled", SKILL_HERETIC, SKILLCAT_DARK },
	{ "dualSwords", SKILL_ENCORE, SKILLCAT_DARK },
	{ "Crossbow", SKILL_NONE, SKILLCAT_INGENUITY },
	{ "Musket", SKILL_NONE, SKILLCAT_INGENUITY },
	{ "Pilum", SKILL_NONE, SKILLCAT_INGENUITY },
	{ "repeatingCrossbow", SKILL_CROSSBOW, SKILLCAT_INGENUITY },
	{ "Marksman", SKILL_MUSKET, SKILLCAT_INGENUITY },
	{ "Flintlock", SKILL_PILUM, SKILLCAT_INGENUITY },
	{ "Dexterity", SKILL_NONE, SKILLCAT_LITHENESS },
	{ "Evade", SKILL_NONE, SKILLCAT_LITHENESS },
	{ "Nimble", SKILL_DEXTERITY, SKILLCAT_LITHENESS },
	{ "Swift", SKILL_EVADE, SKILLCAT_LITHENESS },
};

const int XP_PER_LEVEL = 100;


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
		skillId_t skill = FindSkill(skillName.c_str());
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

void idPlayer::ResetSkills( void )
{
	for (int i = 0; i < NUM_SKILLS; i++ ) {
		skillUnlocked[i] = false;
	}
	xp = 0;
	level = 1;
	skillPoints = 0;
}	

void idPlayer::SaveSkills(idSaveGame* savefile) const 
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		savefile->WriteBool(skillUnlocked[i]);
	}
		savefile->WriteInt( xp );
		savefile->WriteInt( level );
		savefile->WriteInt( skillPoints );
}

void idPlayer::RestoreSkills(idRestoreGame* savefile) 
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		savefile->ReadBool(skillUnlocked[i]);
	}
		savefile->ReadInt(xp);
		savefile->ReadInt(level);
		savefile->ReadInt(skillPoints);
	

}
	
skillId_t idPlayer::FindSkill(const char *name) const
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		if (idStr::Icmp(skillTable[i].name, name) == 0) {
			return (skillId_t)i;	
		}

	}
	return SKILL_NONE;
}

bool idPlayer::HasSkill(skillId_t skill) const
{
	if (skill < 0 || skill >= NUM_SKILLS ) {
		return false;
	}
	return skillUnlocked[ skill ];
}

bool idPlayer::UnlockSkill( skillId_t skill )
{
	if (skill < 0 || skill >= NUM_SKILLS ) {
		return false;
	}
	if ( HasSkill(skill) ) {
		return false;
	}
	skillId_t needed = skillTable[skill].prerequisite;
	if (needed != SKILL_NONE && !HasSkill(needed)) {
		return false;
	}
	if (skillPoints <= 0) {
		return false;
	}
	if (IsFaithBlocked(skill)) {
		gameLocal.Printf("%s is blocked by your faith\n", skillTable[skill].name);
		return false;
	}
	skillPoints--;
	skillUnlocked[ skill ] = true;
	gameLocal.Printf("unlocked %s, %d points left\n", skillTable[skill].name, skillPoints);
	UpdateSkillTreeGui();
	return true;
}

void idPlayer::UpdateSkillTreeGui(void) 
{
	int state;
	if ( !skillTreeGui ) {
		return;
	}
	for (int i = 0; i < NUM_SKILLS; i++) {
		skillId_t skill = (skillId_t)i;
		skillId_t needed = skillTable[skill].prerequisite;
		if (HasSkill(skill)) {
			state = 2;
		}
		else if ( IsFaithBlocked( skill ) ) {
			state = 0;
		}
		else if ( needed == SKILL_NONE || HasSkill( needed ) ) {
			state = 1;
		}
		else {
			state = 0;
		}
		skillTreeGui->SetStateInt(va("skill_%s", skillTable[i].name), state);
	}
	skillTreeGui->SetStateInt("skillPoints", skillPoints);
	skillTreeGui->SetStateInt("level", level);
	skillTreeGui->SetStateInt("xp", xp);

	int needed = XpForNextLevel();
	skillTreeGui->SetStateFloat( "xpPct", (float)xp / (float)needed );
	skillTreeGui->SetStateString( "xpText", va("%d/%d", xp, needed) );
	skillTreeGui->StateChanged(gameLocal.time);
}

int idPlayer::XpForNextLevel(void) const 
{
	return XP_PER_LEVEL * level;
}

void idPlayer::GainXP(int amount) 
{
	xp += amount;
	while ( xp >= XpForNextLevel() ) {
		xp -= XpForNextLevel();
		level++;
		skillPoints++;
		gameLocal.Printf( "LEVEL UP! Now level %d\n", level );
	}
	UpdateSkillTreeGui();
}

bool idPlayer::HasSkillInCategory(skillCategory_t category) const
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		if (skillTable[i].category == category && HasSkill((skillId_t)i)) {
			return true;
		}
	}
	return false;
}

bool idPlayer::IsFaithBlocked(skillId_t skill) const 
{
	skillCategory_t category = skillTable[skill].category;
	if (category == SKILLCAT_LIGHT && HasSkillInCategory(SKILLCAT_DARK)) {
		return true;
	}
	if (category == SKILLCAT_DARK && HasSkillInCategory(SKILLCAT_LIGHT)) {
		return true;
	}
	return false;
}

void idPlayer::RefundSkills(void) 
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		skillId_t skill = (skillId_t)i;
		if (HasSkill(skill)) {
			skillPoints++;
			skillUnlocked[i] = false;
		}
	}
	UpdateSkillTreeGui();
}