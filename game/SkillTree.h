#ifndef __GAME_SKILLTREE_H__
#define __GAME_SKILLTREE_H__


typedef enum {
	SKILLCAT_PROWESS,
	SKILLCAT_FORTITUDE,
	SKILLCAT_LIGHT,
	SKILLCAT_DARK,
	SKILLCAT_INGENUITY,
	SKILLCAT_LITHENESS,
	NUM_SKILLCATS
} skillCategory_t;

typedef enum {
	SKILL_NONE = -1,

	// Prowess
	SKILL_PARRY,
	SKILL_FIGHTER,
	SKILL_BLOCK,
	SKILL_RIPOSTE,
	SKILL_WARRIOR,
	SKILL_SIDESTEP,

	// Fortitude
	SKILL_UNBURDENED,
	SKILL_BRAVE,
	SKILL_OUTLASTING,
	SKILL_VIGOR,

	// Light
	SKILL_GREATSWORD,
	SKILL_BLESSEDPOWDER,
	SKILL_SPEAR,
	SKILL_THELIGHT,
	SKILL_PALADIN,
	SKILL_THEMANIFESTO,

	// Dark
	SKILL_LONGSWORD,
	SKILL_DUALDAGGERS,
	SKILL_HERETIC,
	SKILL_ENCORE,
	SKILL_IMPOSSIBLYSKILLED,
	SKILL_DUALSWORDS,

	// Ingenuity
	SKILL_CROSSBOW,
	SKILL_MUSKET,
	SKILL_PILUM,
	SKILL_REPEATINGCROSSBOW,
	SKILL_MARKSMAN,
	SKILL_FLINTLOCK,

	// Litheness
	SKILL_DEXTERITY,
	SKILL_EVADE,
	SKILL_NIMBLE,
	SKILL_SWIFT,
	NUM_SKILLS
} skillId_t;

typedef struct {
	const char* name;
	skillId_t prerequisite;
	skillCategory_t category;
	const char* weapon;	
} skillDef_t;

class idPlayerSkills 
{
	public:
		void Reset(void);
		void Save(idSaveGame* savefile) const;
		void Restore(idRestoreGame* savefile);

		bool Has(skillId_t skill) const;
		bool HasInCategory(skillCategory_t category) const;
		bool IsFaithBlocked(skillId_t skill) const;
		skillId_t Find(const char* name) const;
		int GetState(skillId_t skill) const;
		const char* GetName(skillId_t skill) const;
		int XpForNextLevel(void) const;

		int GetXP(void) const;
		int GetLevel(void) const;
		int GetPoints(void) const;

		bool Unlock(skillId_t skill);
		int Refund(void);
		int GainXP(int amount);
		const char* GetWeapon(skillId_t skill) const;

	private:
		bool unlocked[NUM_SKILLS];
		int xp;
		int level;
		int points;

};

#endif
