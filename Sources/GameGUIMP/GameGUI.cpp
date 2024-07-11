/* Copyright (c) 2002-2012 Croteam Ltd. 
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// GameGUI.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#if defined(SE1_STATIC_BUILD)
  #define GAMEGUI_DLL_NAME NULL
#elif defined(NDEBUG)
  #define GAMEGUI_DLL_NAME "GameGUIMP.dll"
#else
  #define GAMEGUI_DLL_NAME "GameGUIMPD.dll"
#endif

static int iDialogResult;

#define CALL_DIALOG( class_name, dlg_name)                        \
  try {                                                           \
    _pGame->Load_t();                                             \
  }                                                               \
  catch( char *pError) {                                          \
    (void) pError;                                                \
  }                                                               \
  HANDLE hOldResource = AfxGetResourceHandle();                   \
  class_name dlg_name;                                            \
  AfxSetResourceHandle( GetModuleHandleA(GAMEGUI_DLL_NAME) );     \
  iDialogResult = dlg_name.DoModal();                             \
  AfxSetResourceHandle( (HINSTANCE) hOldResource);                \
  if( iDialogResult == IDOK)                                      \
  try {                                                           \
    _pGame->Save_t();                                             \
  }                                                               \
  catch( char *pError) {                                          \
    AfxMessageBox( CString(pError));                              \
    iDialogResult = IDCANCEL;                                     \
  }

/*
 We cannot use dllmain if using MFC.
 See MSDN article "Regular DLLs Dynamically Linked to MFC" if initialization is needed.

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
*/

/////////////////////////////////////////////////////////////////////////////
// global routines called trough game's application menu

void CGameGUI::OnInvokeConsole(void)
{
  CALL_DIALOG( CDlgConsole, dlgConsole);
}

void CGameGUI::OnPlayerSettings(void)
{
  CALL_DIALOG( CDlgPlayerSettings, dlgPlayerSettings);
}

void CGameGUI::OnAudioQuality(void)
{
  CALL_DIALOG( CDlgAudioQuality, dlgAudioQuality);
}

void CGameGUI::OnVideoQuality(void)
{
  CALL_DIALOG( CDlgVideoQuality, dlgVideoQuality);
}

void CGameGUI::OnSelectPlayerAndControls(void)
{
  CALL_DIALOG( CDlgSelectPlayer, dlgSelectPlayerAndControls);
}
