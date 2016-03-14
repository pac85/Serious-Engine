/* Copyright (c) 2002-2012 Croteam Ltd. All rights reserved. */

#include "StdH.h"
#include <Engine/CurrentVersion.h>
#include "MenuPrinting.h"
#include "LevelInfo.h"
#include "MenuStuff.h"
#include "MSplitStart.h"

#define TRIGGER_MG(mg, y, up, down, text, astr) \
	mg.mg_pmgUp = &up; \
	mg.mg_pmgDown = &down; \
	mg.mg_boxOnScreen = BoxMediumRow(y); \
	gm_lhGadgets.AddTail(mg.mg_lnNode); \
	mg.mg_astrTexts = astr; \
	mg.mg_ctTexts = sizeof(astr) / sizeof(astr[0]); \
	mg.mg_iSelected = 0; \
	mg.mg_strLabel = text; \
	mg.mg_strValue = astr[0];

extern INDEX ctGameTypeRadioTexts;
extern void UpdateSplitLevel(INDEX iDummy);


void CSplitStartMenu::Initialize_t(void)
{
	// intialize split screen menu
	gm_mgTitle.mg_boxOnScreen = BoxTitle();
	gm_mgTitle.mg_strText = TRANS("START SPLIT SCREEN");
	gm_lhGadgets.AddTail(gm_mgTitle.mg_lnNode);

	// game type trigger
	TRIGGER_MG(gm_mgGameType, 0,
		gm_mgStart, gm_mgDifficulty, TRANS("Game type:"), astrGameTypeRadioTexts);
	gm_mgGameType.mg_ctTexts = ctGameTypeRadioTexts;
	gm_mgGameType.mg_strTip = TRANS("choose type of multiplayer game");
	gm_mgGameType.mg_pOnTriggerChange = &UpdateSplitLevel;

	// difficulty trigger
	TRIGGER_MG(gm_mgDifficulty, 1,
		gm_mgGameType, gm_mgLevel, TRANS("Difficulty:"), astrDifficultyRadioTexts);
	gm_mgDifficulty.mg_strTip = TRANS("choose difficulty level");

	// level name
	gm_mgLevel.mg_strText = "";
	gm_mgLevel.mg_strLabel = TRANS("Level:");
	gm_mgLevel.mg_boxOnScreen = BoxMediumRow(2);
	gm_mgLevel.mg_bfsFontSize = BFS_MEDIUM;
	gm_mgLevel.mg_iCenterI = -1;
	gm_mgLevel.mg_pmgUp = &gm_mgDifficulty;
	gm_mgLevel.mg_pmgDown = &gm_mgOptions;
	gm_mgLevel.mg_strTip = TRANS("choose the level to start");
	gm_mgLevel.mg_pActivatedFunction = NULL;
	gm_lhGadgets.AddTail(gm_mgLevel.mg_lnNode);

	// options button
	gm_mgOptions.mg_strText = TRANS("Game options");
	gm_mgOptions.mg_boxOnScreen = BoxMediumRow(3);
	gm_mgOptions.mg_bfsFontSize = BFS_MEDIUM;
	gm_mgOptions.mg_iCenterI = 0;
	gm_mgOptions.mg_pmgUp = &gm_mgLevel;
	gm_mgOptions.mg_pmgDown = &gm_mgStart;
	gm_mgOptions.mg_strTip = TRANS("adjust game rules");
	gm_mgOptions.mg_pActivatedFunction = NULL;
	gm_lhGadgets.AddTail(gm_mgOptions.mg_lnNode);

	// start button
	gm_mgStart.mg_bfsFontSize = BFS_LARGE;
	gm_mgStart.mg_boxOnScreen = BoxBigRow(4);
	gm_mgStart.mg_pmgUp = &gm_mgOptions;
	gm_mgStart.mg_pmgDown = &gm_mgGameType;
	gm_mgStart.mg_strText = TRANS("START");
	gm_lhGadgets.AddTail(gm_mgStart.mg_lnNode);
	gm_mgStart.mg_pActivatedFunction = NULL;
}

void CSplitStartMenu::StartMenu(void)
{
	extern INDEX sam_bMentalActivated;
	gm_mgDifficulty.mg_ctTexts = sam_bMentalActivated ? 6 : 5;

	gm_mgGameType.mg_iSelected = Clamp(_pShell->GetINDEX("gam_iStartMode"), 0L, ctGameTypeRadioTexts - 1L);
	gm_mgGameType.ApplyCurrentSelection();
	gm_mgDifficulty.mg_iSelected = _pShell->GetINDEX("gam_iStartDifficulty") + 1;
	gm_mgDifficulty.ApplyCurrentSelection();

	// clamp maximum number of players to at least 4
	_pShell->SetINDEX("gam_ctMaxPlayers", ClampDn(_pShell->GetINDEX("gam_ctMaxPlayers"), 4L));

	UpdateSplitLevel(0);
	CGameMenu::StartMenu();
}

void CSplitStartMenu::EndMenu(void)
{
	_pShell->SetINDEX("gam_iStartDifficulty", gm_mgDifficulty.mg_iSelected - 1);
	_pShell->SetINDEX("gam_iStartMode", gm_mgGameType.mg_iSelected);

	CGameMenu::EndMenu();
}