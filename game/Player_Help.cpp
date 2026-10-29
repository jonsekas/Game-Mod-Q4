#include "../idlib/precompiled.h"
#pragma hdrstop

#include "Game_local.h"


void idPlayer::OpenHelp(void)
{
	helpOpen = true;
	CloseSkillTree();
	if (helpGui) {
		helpGui->Activate(true, gameLocal.time);
		helpGui->SetCursor(320, 240);

	}
}

void idPlayer::CloseHelp(void)
{
	helpOpen = false;
	if (helpGui) {
		helpGui->Activate(false, gameLocal.time);
	}

}

void idPlayer::ToggleHelp(void)
{
	if (helpOpen) {
		CloseHelp();
	}
	else {
		OpenHelp();
	}
}

bool idPlayer::DrawHelp(void)
{
	if (helpGui && helpOpen) {
		helpGui->Redraw(gameLocal.time);
		helpGui->DrawCursor();
		return true;
	}
	return false;
}


