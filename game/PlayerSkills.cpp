#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"


static const skillDef_t skillTable[NUM_SKILLS] = {
	{ "Parry", SKILL_NONE, SKILLCAT_PROWESS, NULL },
	{ "Fighter", SKILL_NONE, SKILLCAT_PROWESS, NULL },
	{ "Block", SKILL_NONE, SKILLCAT_PROWESS, NULL },
	{ "Riposte", SKILL_PARRY, SKILLCAT_PROWESS, NULL },
	{ "Warrior", SKILL_FIGHTER, SKILLCAT_PROWESS, NULL  },
	{ "Sidestep", SKILL_BLOCK, SKILLCAT_PROWESS, NULL },
	{ "Unburdened", SKILL_NONE, SKILLCAT_FORTITUDE, NULL },
	{ "Brave", SKILL_NONE, SKILLCAT_FORTITUDE, NULL  },
	{ "Outlasting", SKILL_UNBURDENED, SKILLCAT_FORTITUDE, NULL },
	{ "Vigor", SKILL_BRAVE, SKILLCAT_FORTITUDE, NULL },
	{ "Greatsword", SKILL_NONE, SKILLCAT_LIGHT, "weapon_greatsword" },
	{ "blessedPowder", SKILL_NONE, SKILLCAT_LIGHT, NULL  },
	{ "Spear", SKILL_GREATSWORD, SKILLCAT_LIGHT, "weapon_spear" },
	{ "theLight", SKILL_BLESSEDPOWDER, SKILLCAT_LIGHT, NULL  },
	{ "Paladin", SKILL_SPEAR, SKILLCAT_LIGHT, NULL  },
	{ "theManifesto", SKILL_THELIGHT, SKILLCAT_LIGHT, NULL  },
	{ "Longsword", SKILL_NONE, SKILLCAT_DARK, "weapon_longsword" },
	{ "dualDaggers", SKILL_NONE, SKILLCAT_DARK, "weapon_dualdaggers" },
	{ "Heretic", SKILL_LONGSWORD, SKILLCAT_DARK, NULL  },
	{ "Encore", SKILL_DUALDAGGERS, SKILLCAT_DARK, NULL  },
	{ "impossiblySkilled", SKILL_HERETIC, SKILLCAT_DARK, NULL },
	{ "dualSwords", SKILL_ENCORE, SKILLCAT_DARK, "weapon_dualswords" },
	{ "Crossbow", SKILL_NONE, SKILLCAT_INGENUITY, "weapon_crossbow"  },
	{ "Musket", SKILL_NONE, SKILLCAT_INGENUITY, "weapon_musket" },
	{ "Pilum", SKILL_NONE, SKILLCAT_INGENUITY, "weapon_pilum"  },
	{ "repeatingCrossbow", SKILL_CROSSBOW, SKILLCAT_INGENUITY, "weapon_repeatingcrossbow" },
	{ "Marksman", SKILL_MUSKET, SKILLCAT_INGENUITY, NULL  },
	{ "Flintlock", SKILL_PILUM, SKILLCAT_INGENUITY, "weapon_flintlock"  },
	{ "Dexterity", SKILL_NONE, SKILLCAT_LITHENESS, NULL  },
	{ "Evade", SKILL_NONE, SKILLCAT_LITHENESS, NULL },
	{ "Nimble", SKILL_DEXTERITY, SKILLCAT_LITHENESS, NULL  },
	{ "Swift", SKILL_EVADE, SKILLCAT_LITHENESS, NULL },
};

const int XP_PER_LEVEL = 100;

void idPlayerSkills::Reset(void)
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		unlocked[i] = false;
	}
	xp = 0;
	level = 1;
	points = 0;
}

void idPlayerSkills::Save(idSaveGame* savefile) const
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		savefile->WriteBool(unlocked[i]);
	}
	savefile->WriteInt(xp);
	savefile->WriteInt(level);
	savefile->WriteInt(points);
}

void idPlayerSkills::Restore(idRestoreGame* savefile)
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		savefile->ReadBool(unlocked[i]);
	}
	savefile->ReadInt(xp);
	savefile->ReadInt(level);
	savefile->ReadInt(points);


}

skillId_t idPlayerSkills::Find(const char* name) const
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		if (idStr::Icmp(skillTable[i].name, name) == 0) {
			return (skillId_t)i;
		}

	}
	return SKILL_NONE;
}

bool idPlayerSkills::Has(skillId_t skill) const
{
	if (skill < 0 || skill >= NUM_SKILLS) {
		return false;
	}
	return unlocked[skill];
}

bool idPlayerSkills::Unlock(skillId_t skill)
{
	if (skill < 0 || skill >= NUM_SKILLS) {
		return false;
	}
	if (Has(skill)) {
		return false;
	}
	skillId_t needed = skillTable[skill].prerequisite;
	if (needed != SKILL_NONE && !Has(needed)) {
		return false;
	}
	if (points <= 0) {
		return false;
	}
	if (IsFaithBlocked(skill)) {
		gameLocal.Printf("%s is blocked by your faith\n", skillTable[skill].name);
		return false;
	}
	points--;
	unlocked[skill] = true;
	gameLocal.Printf("unlocked %s, %d points left\n", skillTable[skill].name, points);
	return true;
}

int idPlayerSkills::XpForNextLevel(void) const
{
	return XP_PER_LEVEL * level;
}

int idPlayerSkills::GainXP(int amount)
{
	int levelsGained = 0;
	xp += amount;
	while (xp >= XpForNextLevel()) {
		xp -= XpForNextLevel();
		level++;
		points++;
		levelsGained++;
		gameLocal.Printf("LEVEL UP! Now level %d\n", level);
	}
	return levelsGained;
}

bool idPlayerSkills::HasInCategory(skillCategory_t category) const
{
	for (int i = 0; i < NUM_SKILLS; i++) {
		if (skillTable[i].category == category && Has((skillId_t)i)) {
			return true;
		}
	}
	return false;
}

bool idPlayerSkills::IsFaithBlocked(skillId_t skill) const
{
	skillCategory_t category = skillTable[skill].category;
	if (category == SKILLCAT_LIGHT && HasInCategory(SKILLCAT_DARK)) {
		return true;
	}
	if (category == SKILLCAT_DARK && HasInCategory(SKILLCAT_LIGHT)) {
		return true;
	}
	return false;
}

int idPlayerSkills::Refund(void)
{
	int refunded = 0;
	for (int i = 0; i < NUM_SKILLS; i++) {
		skillId_t skill = (skillId_t)i;
		if (Has(skill)) {
			points++;
			unlocked[i] = false;
			refunded++;
		}
	}
	return refunded;
}

int idPlayerSkills::GetState(skillId_t skill) const
{
	skillId_t needed = skillTable[skill].prerequisite;
	if (Has(skill)) {
		return 2;
	}
	if (IsFaithBlocked(skill)) {
		return 0;
	}
	if (needed == SKILL_NONE || Has(needed)) {
		return 1;
	}
	return 0;
}

const char* idPlayerSkills::GetName(skillId_t skill) const 
{
	return skillTable[skill].name;
}

int idPlayerSkills::GetXP(void) const 
{ 
	return xp; 
}
int idPlayerSkills::GetLevel(void) const 
{ 
	return level; 
}
int idPlayerSkills::GetPoints(void) const 
{ 
	return points; 
}

const char* idPlayerSkills::GetWeapon(skillId_t skill) const
{
	if (skill < 0 || skill >= NUM_SKILLS) {
		return NULL;
	}
	return skillTable[skill].weapon;
}