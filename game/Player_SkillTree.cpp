#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"

void idPlayer::CloseSkillTree(void)
{
	skillTreeOpen = false;
	if (skillTreeGui) {
		skillTreeGui->Activate(false, gameLocal.time);
	}

}