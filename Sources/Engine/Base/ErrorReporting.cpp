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

#include "StdH.h"

#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/ErrorTable.h>
#include <Engine/Base/Translation.h>

#include <Engine/Base/FileName.h>
#include <Engine/Base/CTString.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Console_internal.h>

#include <Engine/Graphics/Adapter.h>

INDEX con_bNoWarnings = 0;

// global handle for application window in full-screen mode
extern OS::Window _hwndCurrent;
extern BOOL _bFullScreen;

#if !SE1_EXF_VERIFY_VA_IN_PRINTF

/*
 * Report error and terminate program.
 */
static BOOL _bInFatalError = FALSE;
void FatalError(const char *strFormat, ...)
{
  // disable recursion
  if (_bInFatalError) return;
  _bInFatalError = TRUE;

  // reset default windows display mode first 
  // (this is a low overhead and shouldn't allocate memory)
  CDS_ResetMode();

#if !SE1_PREFER_SDL
  // hide fullscreen window if any
  if( _bFullScreen) {
    // must do minimize first - don't know why :(
    ShowWindow( _hwndCurrent, SW_MINIMIZE);
    ShowWindow( _hwndCurrent, SW_HIDE);
  }

#else
  // [Cecil] SDL: Destroy the window
  _hwndCurrent.Destroy();
#endif

  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);

  if (_pConsole!=NULL) {
    // print the buffer to the console
    CPutString(TRANS("FatalError:\n"));
    CPutString(strBuffer.ConstData());
    // make sure the console log was written safely
    _pConsole->CloseLog();
  }

#if !SE1_PREFER_SDL
  // create message box with just OK button
  MessageBoxA(NULL, strBuffer.ConstData(), TRANS("Fatal Error"),
    MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);

  extern void EnableWindowsKeys(void);
  EnableWindowsKeys();

#else
  // [Cecil] SDL: Show fatal error
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, TRANS("Fatal Error"), strBuffer.ConstData(), NULL);
  SDL_Quit();
#endif

  _bInFatalError = FALSE;

  // exit program
  exit(EXIT_FAILURE);
}

// [Cecil] Report error without terminating the program
void ErrorMessage(const char *strFormat, ...) {
  // Format the message
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);

  // Print it to console
  CPutString(TRANS("Error:\n"));
  CPrintF("%s\n", strBuffer.ConstData());

  #if !SE1_PREFER_SDL
    MessageBoxA(NULL, strBuffer.ConstData(), TRANS("Error"), MB_OK|MB_ICONHAND|MB_SETFOREGROUND|MB_TASKMODAL);
  #else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, TRANS("Error"), strBuffer.ConstData(), _hwndCurrent);
  #endif
};

/*
 * Report warning without terminating program (stops program until user responds).
 */
void WarningMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);

  // print it to console
  CPrintF("%s\n", strBuffer.ConstData());
  // if warnings are enabled
  if( !con_bNoWarnings) {
  #if !SE1_PREFER_SDL
    // create message box
    MessageBoxA(NULL, strBuffer.ConstData(), TRANS("Warning"), MB_OK|MB_ICONEXCLAMATION|MB_SETFOREGROUND|MB_TASKMODAL);

  #else
    // [Cecil] SDL: Show warning
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, TRANS("Warning"), strBuffer.ConstData(), _hwndCurrent);
  #endif
  }
}

void InfoMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);

  // print it to console
  CPrintF("%s\n", strBuffer.ConstData());

#if !SE1_PREFER_SDL
  // create message box
  MessageBoxA(NULL, strBuffer.ConstData(), TRANS("Information"), MB_OK|MB_ICONINFORMATION|MB_SETFOREGROUND|MB_TASKMODAL);

#else
  // [Cecil] SDL: Show information
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, TRANS("Information"), strBuffer.ConstData(), _hwndCurrent);
#endif
}

/* Ask user for yes/no answer(stops program until user responds). */
BOOL YesNoMessage(const char *strFormat, ...)
{
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat);
  CTString strBuffer;
  strBuffer.VPrintF(strFormat, arg);

  // print it to console
  CPrintF("%s\n", strBuffer.ConstData());

#if !SE1_PREFER_SDL
  // create message box
  return MessageBoxA(NULL, strBuffer.ConstData(), TRANS("Question"), MB_YESNO|MB_ICONQUESTION|MB_SETFOREGROUND|MB_TASKMODAL)==IDYES;

#else
  // [Cecil] SDL: Show question
  const SDL_MessageBoxButtonData aButtons[] = {
    { SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, TRANS("No") },
    { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, TRANS("Yes") },
  };

  const SDL_MessageBoxData msgbox = {
    SDL_MESSAGEBOX_INFORMATION, _hwndCurrent,
    TRANS("Question"), strBuffer, SDL_arraysize(aButtons), aButtons, NULL,
  };

  int iResponse = 0;
  int iOK = SDL_ShowMessageBox(&msgbox, &iResponse);

  return (iOK != -1 && iResponse == 1);
#endif
}

/*
 * Throw an exception of formatted string.
 */
void ThrowF_t(const char *strFormat, ...)  // throws char *
{
  const SLONG slBufferSize = 256;
  char strBuffer[slBufferSize+1];
  // format the message in buffer
  va_list arg;
  va_start(arg, strFormat); // variable arguments start after this argument
  _vsnprintf(strBuffer, slBufferSize, strFormat, arg);
  throw strBuffer;
}

#endif // SE1_EXF_VERIFY_VA_IN_PRINTF

/*
 * Get the name string for error code.
 */
const char *ErrorName(const struct ErrorTable *pet, SLONG ulErrCode)
{
  for (INDEX i=0; i<pet->et_Count; i++) {
    if (pet->et_Errors[i].ec_Code == ulErrCode) {
      return pet->et_Errors[i].ec_Name;
    }
  }
  return TRANS("CROTEAM_UNKNOWN");
}

/*
 * Get the description string for error code.
 */
const char *ErrorDescription(const struct ErrorTable *pet, SLONG ulErrCode)
{
  for (INDEX i=0; i<pet->et_Count; i++) {
    if (pet->et_Errors[i].ec_Code == ulErrCode) {
      return pet->et_Errors[i].ec_Description;
    }
  }
  return TRANS("Unknown error");
}

/*
 * Get the description string for windows error code.
 */
const CTString GetWindowsError(DWORD dwWindowsErrorCode)
{
#if SE1_WIN
  // buffer to receive error description
  LPVOID lpMsgBuf;
  // call function that will prepare text abount given windows error code
  DWORD dwSuccess = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, dwWindowsErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&lpMsgBuf, 0, NULL);

  // create result CTString from prepared message
  CTString strResultMessage;

  // [Cecil] Make sure the message could be formatted at all
  if (dwSuccess != 0) {
    // copy the result
    strResultMessage = (char *)lpMsgBuf;
    // free the windows message buffer
    LocalFree(lpMsgBuf);

  } else {
    // set our message about the failure
    strResultMessage.PrintF(
      TRANS("Cannot format error message!\n"
      "Original error code: %d,\n"
      "Formatting error code: %d.\n"),
      dwWindowsErrorCode, GetLastError());
  }

  return strResultMessage;

#else
  ASSERTALWAYS("GetWindowsError() is only supported on Windows OS!");
  return "GetWindowsError() is only supported on Windows OS!";
#endif
}

// must be in separate function to disable stupid optimizer
void Breakpoint(void)
{
// [Cecil] Prioritize old compiler
#if SE1_OLD_COMPILER || SE1_USE_ASM
  __asm int 0x03;
#else
  // [Cecil] SDL: Trigger breakpoint universally
  SDL_TriggerBreakpoint();
#endif
}
