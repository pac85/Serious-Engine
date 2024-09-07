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

#include <Engine/Base/Timer.h>
#include <Engine/Base/Input.h>
#include <Engine/Base/Translation.h>
#include <Engine/Base/KeyNames.h>
#include <Engine/Math/Functions.h>
#include <Engine/Graphics/ViewPort.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Synchronization.h>

#include <Engine/Base/ErrorReporting.h>

extern INDEX inp_iKeyboardReadingMethod;
extern FLOAT inp_fMouseSensitivity;
extern INDEX inp_bAllowMouseAcceleration;
extern INDEX inp_bMousePrecision;
extern FLOAT inp_fMousePrecisionFactor;
extern FLOAT inp_fMousePrecisionThreshold;
extern FLOAT inp_fMousePrecisionTimeout;
extern FLOAT inp_bInvertMouse;
extern INDEX inp_bFilterMouse;
extern INDEX inp_bAllowPrescan;

INDEX inp_iMButton4Dn = 0x20040;
INDEX inp_iMButton4Up = 0x20000;
INDEX inp_iMButton5Dn = 0x10020;
INDEX inp_iMButton5Up = 0x10000;
INDEX inp_bMsgDebugger = FALSE;
INDEX inp_bForceJoystickPolling = 0;
INDEX inp_ctJoysticksAllowed = 8;
INDEX inp_bAutoDisableJoysticks = 0;

CTString inp_astrAxisTran[MAX_OVERALL_AXES]; // translated names for axis

/*

NOTE: Three different types of key codes are used here:
  1) kid - engine internal type - defined in KeyNames.h
  2) scancode - raw PC keyboard scancodes as returned in keydown/keyup messages
  3) virtkey - virtual key codes used by Windows or SDL

*/

// name that is not translated (international)
#define INTNAME(str) str, ""
// name that is translated
#define TRANAME(str) str, "ETRS" str

// basic key conversion table
struct KeyConversion {
  INDEX kc_iKID;
  INDEX kc_iVirtKey;

  // [Cecil] NOTE: Some keys are offsetted by 256 to account for the "extended" flag that's being set
  // near scancodes when receiving key messages on Windows, otherwise it's impossible to distinguish
  // certain keys, e.g. Left Alt/Right Alt or Enter/Num Enter. As a result, the received scancode is
  // determined with a 0x1FF mask instead of just 0xFF. These scancodes are useless with SDL because
  // SDL is capable of distinguishing the keys when converting between its virtual keys and scancodes.
  INDEX kc_iScanCode;

  const char *kc_strName;
  const char *kc_strNameTrans;
};

static const KeyConversion _akcKeys[] = {
  // reserved for 'no-key-pressed'
  { KID_NONE, -1, -1, TRANAME("None")},

  // numbers row
  { KID_1               , SE1K_1,       2, INTNAME("1")},
  { KID_2               , SE1K_2,       3, INTNAME("2")},
  { KID_3               , SE1K_3,       4, INTNAME("3")},
  { KID_4               , SE1K_4,       5, INTNAME("4")},
  { KID_5               , SE1K_5,       6, INTNAME("5")},
  { KID_6               , SE1K_6,       7, INTNAME("6")},
  { KID_7               , SE1K_7,       8, INTNAME("7")},
  { KID_8               , SE1K_8,       9, INTNAME("8")},
  { KID_9               , SE1K_9,      10, INTNAME("9")},
  { KID_0               , SE1K_0,      11, INTNAME("0")},
  { KID_MINUS           , SE1K_MINUS,  12, INTNAME("-")},
  { KID_EQUALS          , SE1K_EQUALS, 13, INTNAME("=")},

  // 1st alpha row
  { KID_Q               , SE1K_q,            16, INTNAME("Q")},
  { KID_W               , SE1K_w,            17, INTNAME("W")},
  { KID_E               , SE1K_e,            18, INTNAME("E")},
  { KID_R               , SE1K_r,            19, INTNAME("R")},
  { KID_T               , SE1K_t,            20, INTNAME("T")},
  { KID_Y               , SE1K_y,            21, INTNAME("Y")},
  { KID_U               , SE1K_u,            22, INTNAME("U")},
  { KID_I               , SE1K_i,            23, INTNAME("I")},
  { KID_O               , SE1K_o,            24, INTNAME("O")},
  { KID_P               , SE1K_p,            25, INTNAME("P")},
  { KID_LBRACKET        , SE1K_LEFTBRACKET,  26, INTNAME("[")},
  { KID_RBRACKET        , SE1K_RIGHTBRACKET, 27, INTNAME("]")},
  { KID_BACKSLASH       , SE1K_BACKSLASH,    43, INTNAME("\\")},

  // 2nd alpha row
  { KID_A               , SE1K_a,         30, INTNAME("A")},
  { KID_S               , SE1K_s,         31, INTNAME("S")},
  { KID_D               , SE1K_d,         32, INTNAME("D")},
  { KID_F               , SE1K_f,         33, INTNAME("F")},
  { KID_G               , SE1K_g,         34, INTNAME("G")},
  { KID_H               , SE1K_h,         35, INTNAME("H")},
  { KID_J               , SE1K_j,         36, INTNAME("J")},
  { KID_K               , SE1K_k,         37, INTNAME("K")},
  { KID_L               , SE1K_l,         38, INTNAME("L")},
  { KID_SEMICOLON       , SE1K_SEMICOLON, 39, INTNAME(";")},
  { KID_APOSTROPHE      , SE1K_QUOTE,     40, INTNAME("'")},

  // 3rd alpha row
  { KID_Z               , SE1K_z,      44, INTNAME("Z")},
  { KID_X               , SE1K_x,      45, INTNAME("X")},
  { KID_C               , SE1K_c,      46, INTNAME("C")},
  { KID_V               , SE1K_v,      47, INTNAME("V")},
  { KID_B               , SE1K_b,      48, INTNAME("B")},
  { KID_N               , SE1K_n,      49, INTNAME("N")},
  { KID_M               , SE1K_m,      50, INTNAME("M")},
  { KID_COMMA           , SE1K_COMMA,  51, INTNAME(",")},
  { KID_PERIOD          , SE1K_PERIOD, 52, INTNAME(".")},
  { KID_SLASH           , SE1K_SLASH,  53, INTNAME("/")},

  // row with F-keys
  { KID_F1              , SE1K_F1,   59, INTNAME("F1")},
  { KID_F2              , SE1K_F2,   60, INTNAME("F2")},
  { KID_F3              , SE1K_F3,   61, INTNAME("F3")},
  { KID_F4              , SE1K_F4,   62, INTNAME("F4")},
  { KID_F5              , SE1K_F5,   63, INTNAME("F5")},
  { KID_F6              , SE1K_F6,   64, INTNAME("F6")},
  { KID_F7              , SE1K_F7,   65, INTNAME("F7")},
  { KID_F8              , SE1K_F8,   66, INTNAME("F8")},
  { KID_F9              , SE1K_F9,   67, INTNAME("F9")},
  { KID_F10             , SE1K_F10,  68, INTNAME("F10")},
  { KID_F11             , SE1K_F11,  87, INTNAME("F11")},
  { KID_F12             , SE1K_F12,  88, INTNAME("F12")},

  // extra keys
  { KID_ESCAPE          , SE1K_ESCAPE,     1, TRANAME("Escape")},
  { KID_TILDE           , -1,             41, TRANAME("Tilde")},
  { KID_BACKSPACE       , SE1K_BACKSPACE, 14, TRANAME("Backspace")},
  { KID_TAB             , SE1K_TAB,       15, TRANAME("Tab")},
  { KID_CAPSLOCK        , SE1K_CAPSLOCK,  58, TRANAME("Caps Lock")},
  { KID_ENTER           , SE1K_RETURN,    28, TRANAME("Enter")},
  { KID_SPACE           , SE1K_SPACE,     57, TRANAME("Space")},

  // modifier keys
  { KID_LSHIFT          , SE1K_LSHIFT, 42,     TRANAME("Left Shift")},
  { KID_RSHIFT          , SE1K_RSHIFT, 54,     TRANAME("Right Shift")},
  { KID_LCONTROL        , SE1K_LCTRL,  29,     TRANAME("Left Control")},
  { KID_RCONTROL        , SE1K_RCTRL,  29+256, TRANAME("Right Control")},
  { KID_LALT            , SE1K_LALT,   56,     TRANAME("Left Alt")},
  { KID_RALT            , SE1K_RALT,   56+256, TRANAME("Right Alt")},

  // navigation keys
  { KID_ARROWUP         , SE1K_UP,           72+256, TRANAME("Arrow Up")},
  { KID_ARROWDOWN       , SE1K_DOWN,         80+256, TRANAME("Arrow Down")},
  { KID_ARROWLEFT       , SE1K_LEFT,         75+256, TRANAME("Arrow Left")},
  { KID_ARROWRIGHT      , SE1K_RIGHT,        77+256, TRANAME("Arrow Right")},
  { KID_INSERT          , SE1K_INSERT,       82+256, TRANAME("Insert")},
  { KID_DELETE          , SE1K_DELETE,       83+256, TRANAME("Delete")},
  { KID_HOME            , SE1K_HOME,         71+256, TRANAME("Home")},
  { KID_END             , SE1K_END,          79+256, TRANAME("End")},
  { KID_PAGEUP          , SE1K_PAGEUP,       73+256, TRANAME("Page Up")},
  { KID_PAGEDOWN        , SE1K_PAGEDOWN,     81+256, TRANAME("Page Down")},
  { KID_PRINTSCR        , SE1K_PRINTSCREEN,  55+256, TRANAME("Print Screen")},
  { KID_SCROLLLOCK      , SE1K_SCROLLLOCK,   70,     TRANAME("Scroll Lock")},
  { KID_PAUSE           , SE1K_PAUSE,        69,     TRANAME("Pause")},

  // numpad numbers
  { KID_NUM0            , SE1K_KP_0,      82, INTNAME("Num 0")},
  { KID_NUM1            , SE1K_KP_1,      79, INTNAME("Num 1")},
  { KID_NUM2            , SE1K_KP_2,      80, INTNAME("Num 2")},
  { KID_NUM3            , SE1K_KP_3,      81, INTNAME("Num 3")},
  { KID_NUM4            , SE1K_KP_4,      75, INTNAME("Num 4")},
  { KID_NUM5            , SE1K_KP_5,      76, INTNAME("Num 5")},
  { KID_NUM6            , SE1K_KP_6,      77, INTNAME("Num 6")},
  { KID_NUM7            , SE1K_KP_7,      71, INTNAME("Num 7")},
  { KID_NUM8            , SE1K_KP_8,      72, INTNAME("Num 8")},
  { KID_NUM9            , SE1K_KP_9,      73, INTNAME("Num 9")},
  { KID_NUMDECIMAL      , SE1K_KP_PERIOD, 83, INTNAME("Num .")},

  // numpad gray keys
  { KID_NUMLOCK         , SE1K_NUMLOCKCLEAR, 69+256, INTNAME("Num Lock")},
  { KID_NUMSLASH        , SE1K_KP_DIVIDE,    53+256, INTNAME("Num /")},
  { KID_NUMMULTIPLY     , SE1K_KP_MULTIPLY,  55,     INTNAME("Num *")},
  { KID_NUMMINUS        , SE1K_KP_MINUS,     74,     INTNAME("Num -")},
  { KID_NUMPLUS         , SE1K_KP_PLUS,      78,     INTNAME("Num +")},
  { KID_NUMENTER        , SE1K_KP_ENTER,     28+256, TRANAME("Num Enter")},

  // mouse buttons
  { KID_MOUSE1          , SE1K_LBUTTON, -1, TRANAME("Mouse Button 1")},
  { KID_MOUSE2          , SE1K_RBUTTON, -1, TRANAME("Mouse Button 2")},
  { KID_MOUSE3          , SE1K_MBUTTON, -1, TRANAME("Mouse Button 3")},
  { KID_MOUSE4          , -1, -1, TRANAME("Mouse Button 4")},
  { KID_MOUSE5          , -1, -1, TRANAME("Mouse Button 5")},
  { KID_MOUSEWHEELUP    , -1, -1, TRANAME("Mouse Wheel Up")},
  { KID_MOUSEWHEELDOWN  , -1, -1, TRANAME("Mouse Wheel Down")},

  // 2nd mouse buttons
  { KID_2MOUSE1         , -1, -1, TRANAME("2nd Mouse Button 1")},
  { KID_2MOUSE2         , -1, -1, TRANAME("2nd Mouse Button 2")},
  { KID_2MOUSE3         , -1, -1, TRANAME("2nd Mouse Button 3")},
};

// autogenerated fast conversion tables
static INDEX _aiScanToKid[SDL_NUM_SCANCODES];

// make fast conversion tables from the general table
static void MakeConversionTables(void) {
  INDEX i;

  // Clear conversion table
  for (i = 0; i < ARRAYCOUNT(_aiScanToKid); i++) {
    _aiScanToKid[i] = -1;
  }

  for (i = 0; i < ARRAYCOUNT(_akcKeys); i++) {
    const KeyConversion &kc = _akcKeys[i];

    const INDEX iKID  = kc.kc_iKID;
    const INDEX iVirt = kc.kc_iVirtKey;

    if (iVirt >= 0) {
      // [Cecil] NOTE: Cannot use MapVirtualKey() here; see comment near kc_iScanCode field
      INDEX iScan = kc.kc_iScanCode;

      _aiScanToKid[iScan] = iKID;
    }
  }

  // In case some keycode couldn't be mapped to a scancode
  _aiScanToKid[SDL_SCANCODE_UNKNOWN] = -1;
};

// variables for message interception
static int _iMouseZ = 0;
static BOOL _bWheelUp = FALSE;
static BOOL _bWheelDn = FALSE;

CTCriticalSection csInput;

// which keys are pressed, as recorded by message interception (by KIDs)
static UBYTE _abKeysPressed[256];

static HHOOK _hGetMsgHook = NULL;
static HHOOK _hSendMsgHook = NULL;

// set a key according to a keydown/keyup message
static void SetKeyFromMsg(MSG *pMsg, BOOL bDown)
{
  // get scan code
  INDEX iScan = (pMsg->lParam >> 16) & 0x1FF; // (we use the extended bit too!)
  // convert scan code to kid
  INDEX iKID = _aiScanToKid[iScan];

  if (iKID >= 0 && iKID < ARRAYCOUNT(_abKeysPressed)) {
    //CPrintF("%s: %d\n", _pInput->inp_strButtonNames[iKID], bDown);
    _abKeysPressed[iKID] = bDown;
  }
};

static void CheckMessage(MSG *pMsg)
{
  if ( pMsg->message == WM_LBUTTONUP) {
    _abKeysPressed[KID_MOUSE1] = FALSE;
  } else if ( pMsg->message == WM_LBUTTONDOWN || pMsg->message == WM_LBUTTONDBLCLK) {
    _abKeysPressed[KID_MOUSE1] = TRUE;
  } else if ( pMsg->message == WM_RBUTTONUP) {
    _abKeysPressed[KID_MOUSE2] = FALSE;
  } else if ( pMsg->message == WM_RBUTTONDOWN || pMsg->message == WM_RBUTTONDBLCLK) {
    _abKeysPressed[KID_MOUSE2] = TRUE;
  } else if ( pMsg->message == WM_MBUTTONUP) {
    _abKeysPressed[KID_MOUSE3] = FALSE;
  } else if ( pMsg->message == WM_MBUTTONDOWN || pMsg->message == WM_MBUTTONDBLCLK) {
    _abKeysPressed[KID_MOUSE3] = TRUE;

  } else if ( pMsg->message == inp_iMButton4Dn) {
    _abKeysPressed[KID_MOUSE4] = TRUE;
  } else if ( pMsg->message == inp_iMButton4Up) {
    _abKeysPressed[KID_MOUSE4] = FALSE;

  } else if ( pMsg->message == inp_iMButton5Dn) {
    _abKeysPressed[KID_MOUSE5] = TRUE;
  } else if ( pMsg->message == inp_iMButton5Up) {
    _abKeysPressed[KID_MOUSE5] = FALSE;

  } else if (pMsg->message==WM_KEYUP || pMsg->message==WM_SYSKEYUP) {
    SetKeyFromMsg(pMsg, FALSE);
  } else if (pMsg->message==WM_KEYDOWN || pMsg->message==WM_SYSKEYDOWN) {
    SetKeyFromMsg(pMsg, TRUE);
  } else if (inp_bMsgDebugger && pMsg->message >= 0x10000) {
    CPrintF("%08x(%d)\n", pMsg->message, pMsg->message);
  }
}

// procedure called when message is retreived
LRESULT CALLBACK GetMsgProc(
  int nCode,      // hook code
  WPARAM wParam,  // message identifier
  LPARAM lParam)  // mouse coordinates
{
  MSG *pMsg = reinterpret_cast<MSG *>(lParam);
  CheckMessage(pMsg);

  LRESULT retValue = 0;
  retValue = CallNextHookEx( _hGetMsgHook, nCode, wParam, lParam );

  if (wParam == PM_NOREMOVE) {
    return retValue;
  }

  if ( pMsg->message == WM_MOUSEWHEEL) {
    _iMouseZ += SWORD(UWORD(HIWORD(pMsg->wParam)));
  }

  return retValue;
}


// procedure called when message is sent
LRESULT CALLBACK SendMsgProc(
  int nCode,      // hook code
  WPARAM wParam,  // message identifier
  LPARAM lParam)  // mouse coordinates
{
  MSG *pMsg = (MSG*)lParam;
  CheckMessage(pMsg);

  LRESULT retValue = 0;
  retValue = CallNextHookEx( _hSendMsgHook, nCode, wParam, lParam );
  
  return retValue;
}

// pointer to global input object
CInput *_pInput = NULL;

// deafult constructor
CInput::CInput(void)
{
  // disable control scaning
  inp_bInputEnabled = FALSE;
  inp_bPollJoysticks = FALSE;
  inp_bLastPrescan = FALSE;
  // clear key buffer
  for( INDEX iButton=0; iButton<MAX_OVERALL_BUTTONS; iButton++)
  {
    inp_ubButtonsBuffer[ iButton] = 0;
  }
  // clear axis relative and absolute values
  for( INDEX iAxis=0; iAxis<MAX_OVERALL_AXES; iAxis++)
  {
    inp_caiAllAxisInfo[ iAxis].cai_fReading  = 0.0f;
    inp_caiAllAxisInfo[ iAxis].cai_bExisting = FALSE;
  }

  MakeConversionTables();
};

// Destructor
CInput::~CInput() {
  Mouse2_Clear(); // [Cecil]
};

/*
 * Sets names of keys on keyboard
 */
void CInput::SetKeyNames( void)
{
  // set name "None" for all keys, known keys will override this default name
  {for (INDEX iKey = 0; iKey < ARRAYCOUNT(inp_strButtonNames); iKey++) {
    inp_strButtonNames[iKey] = "None";
    inp_strButtonNamesTra[iKey] = TRANS("None");
  }}

  // for each Key
  {for (INDEX iKey=0; iKey<ARRAYCOUNT(_akcKeys); iKey++) {
    const KeyConversion &kc = _akcKeys[iKey];

    // set the name
    if (kc.kc_strName!=NULL) {
      inp_strButtonNames[kc.kc_iKID] = kc.kc_strName;
      if (strlen(kc.kc_strNameTrans)==0) {
        inp_strButtonNamesTra[kc.kc_iKID] = kc.kc_strName;
      } else {
        inp_strButtonNamesTra[kc.kc_iKID] = TranslateConst(kc.kc_strNameTrans, 4);
      }
    }
  }}

  // -------- Enumerate known axis -------------
  // no axis as axis type 0
  inp_caiAllAxisInfo[0].cai_strAxisName = "None";
  inp_astrAxisTran[  0] = TRANS("None");
  // mouse axis occupy types from 1 up to 3
  inp_caiAllAxisInfo[1].cai_strAxisName = "mouse X";
  inp_astrAxisTran[  1] = TRANS("mouse X");
  inp_caiAllAxisInfo[2].cai_strAxisName = "mouse Y";
  inp_astrAxisTran[  2] = TRANS("mouse Y");
  inp_caiAllAxisInfo[3].cai_strAxisName = "mouse Z";
  inp_astrAxisTran[  3] = TRANS("mouse Z");
  inp_caiAllAxisInfo[4].cai_strAxisName = "2nd mouse X";
  inp_astrAxisTran[  4] = TRANS("2nd mouse X");
  inp_caiAllAxisInfo[5].cai_strAxisName = "2nd mouse Y";
  inp_astrAxisTran[  5] = TRANS("2nd mouse Y");

  // [Cecil] Set joystick names separately
  SetJoystickNames();
}

/*
 * Initializes all available devices and enumerates available controls
 */
void CInput::Initialize( void )
{
  CPutString(TRANS("Detecting input devices...\n"));

  SetKeyNames();
  Mouse2_Clear(); // [Cecil]

  CPutString("\n");
}


/*
 * Enable direct input
 */

void CInput::EnableInput(CViewPort *pvp)
{
  EnableInput(pvp->vp_hWnd);
}


void CInput::EnableInput(OS::Window hwnd)
{
  // skip if already enabled
  if( inp_bInputEnabled) return;

#if SE1_PREFER_SDL
  // [Cecil] FIXME: Get HWND from SDL_Window or...
  // [Cecil] TODO: Rewrite input using SDL
  HWND hwndCurrent = GetActiveWindow();
#else
  HWND hwndCurrent = hwnd;
#endif

  // get window rectangle
  RECT rectClient;
  GetClientRect(hwndCurrent, &rectClient);
  POINT pt;
  pt.x = pt.y = 0;
  ClientToScreen(hwndCurrent, &pt);
  OffsetRect(&rectClient, pt.x, pt.y);

  // remember mouse pos
  OS::GetMouseState(&inp_aOldMousePos[0], &inp_aOldMousePos[1], FALSE);
  // set mouse clip region
  ClipCursor(&rectClient);
  // determine screen center position
  inp_slScreenCenterX = (rectClient.left + rectClient.right) / 2;
  inp_slScreenCenterY = (rectClient.top + rectClient.bottom) / 2;

  // clear mouse from screen
  while (OS::ShowCursor(FALSE) >= 0);
  // save system mouse settings
  SystemParametersInfo(SPI_GETMOUSE, 0, &inp_mscMouseSettings, 0);
  // set new mouse speed
  if (!inp_bAllowMouseAcceleration) {
    MouseSpeedControl mscNewSetting = { 0, 0, 0};
    SystemParametersInfo(SPI_SETMOUSE, 0, &mscNewSetting, 0);
  }
  // set cursor position to screen center
  SetCursorPos(inp_slScreenCenterX, inp_slScreenCenterY);

  _hGetMsgHook  = SetWindowsHookEx(WH_GETMESSAGE, &GetMsgProc, NULL, GetCurrentThreadId());
  _hSendMsgHook = SetWindowsHookEx(WH_CALLWNDPROC, &SendMsgProc, NULL, GetCurrentThreadId());

  // if required, try to enable 2nd mouse
  Mouse2_Startup(); // [Cecil]

  // clear button's buffer
  memset( _abKeysPressed, 0, sizeof( _abKeysPressed));

  // This can be enabled to pre-read the state of currently pressed keys
  // for snooping methods, since they only detect transitions.
  // That has effect of detecting key presses for keys that were held down before
  // enabling.
  // the entire thing is disabled because it caused last menu key to re-apply in game.
#if 0
  // for each Key
  {for (INDEX iKey=0; iKey<ARRAYCOUNT(_akcKeys); iKey++) {
    struct KeyConversion &kc = _akcKeys[iKey];
    // get codes
    INDEX iKID  = kc.kc_iKID;
    INDEX iScan = kc.kc_iScanCode;
    INDEX iVirt = kc.kc_iVirtKey;

    // if there is a valid virtkey
    if (iVirt>=0) {
      // transcribe if modifier
      if (iVirt == VK_LSHIFT) {
        iVirt = VK_SHIFT;
      }
      if (iVirt == VK_LCONTROL) {
        iVirt = VK_CONTROL;
      }
      if (iVirt == VK_LMENU) {
        iVirt = VK_MENU;
      }
      // is state is pressed
      if (OS::GetAsyncKeyState(iVirt) & 0x8000) {
        // mark it as pressed
        _abKeysPressed[iKID] = 0xFF;
      }
    }
  }}
#endif
  
  // remember current status
  inp_bInputEnabled = TRUE;
  inp_bPollJoysticks = FALSE;
}


/*
 * Disable direct input
 */
void CInput::DisableInput( void)
{
  // skip if allready disabled
  if( !inp_bInputEnabled) return;
  
  UnhookWindowsHookEx(_hGetMsgHook);
  UnhookWindowsHookEx(_hSendMsgHook);

  // set mouse clip region to entire screen
  ClipCursor(NULL);
  // restore mouse pos
  SetCursorPos(inp_aOldMousePos[0], inp_aOldMousePos[1]);

  // show mouse on screen
  while (OS::ShowCursor(TRUE) < 0);
  // set system mouse settings
  SystemParametersInfo(SPI_SETMOUSE, 0, &inp_mscMouseSettings, 0);

  // eventually disable 2nd mouse
  Mouse2_Shutdown(); // [Cecil]

  // remember current status
  inp_bInputEnabled = FALSE;
  inp_bPollJoysticks = FALSE;
}


/*
 * Scan states of all available input sources
 */
void CInput::GetInput(BOOL bPreScan)
{
//  CTSingleLock sl(&csInput, TRUE);

  if (!inp_bInputEnabled) {
    return;
  }

  if (bPreScan && !inp_bAllowPrescan) {
    return;
  }

  // if not pre-scanning
  if (!bPreScan) {
    // clear button's buffer
    memset( inp_ubButtonsBuffer, 0, sizeof( inp_ubButtonsBuffer));

    // for each Key
    for (INDEX iKey=0; iKey<ARRAYCOUNT(_akcKeys); iKey++) {
      const KeyConversion &kc = _akcKeys[iKey];
      // get codes
      INDEX iKID  = kc.kc_iKID;
      INDEX iVirt = kc.kc_iVirtKey;

      // if reading async keystate
      if (inp_iKeyboardReadingMethod == 0) {
        // if there is a valid virtkey
        if (iVirt >= 0) {
          BOOL bKeyPressed = FALSE;

          // transcribe if modifier
          if (iVirt == VK_LSHIFT)   iVirt = VK_SHIFT;
          if (iVirt == VK_LCONTROL) iVirt = VK_CONTROL;
          if (iVirt == VK_LMENU)    iVirt = VK_MENU;

          bKeyPressed = !!(OS::GetKeyState(iVirt) & 0x8000);

          // is state is pressed
          if (bKeyPressed) {
            // mark it as pressed
            inp_ubButtonsBuffer[iKID] = 0xFF;
          }
        }

      // if snooping messages
      } else if (_abKeysPressed[iKID]) {
        // mark it as pressed
        inp_ubButtonsBuffer[iKID] = 0xFF;
      }
    }
  }

  // read mouse position
  int iMouseX, iMouseY;
  OS::GetMouseState(&iMouseX, &iMouseY, FALSE);

  {
    FLOAT fDX = FLOAT(iMouseX - inp_slScreenCenterX);
    FLOAT fDY = FLOAT(iMouseY - inp_slScreenCenterY);

    FLOAT fSensitivity = inp_fMouseSensitivity;
    if( inp_bAllowMouseAcceleration) fSensitivity *= 0.25f;

    FLOAT fD = Sqrt(fDX*fDX+fDY*fDY);
    if (inp_bMousePrecision) {
      static FLOAT _tmTime = 0.0f;
      if( fD<inp_fMousePrecisionThreshold) _tmTime += 0.05f;
      else _tmTime = 0.0f;
      if( _tmTime>inp_fMousePrecisionTimeout) fSensitivity /= inp_fMousePrecisionFactor;
    }

    static FLOAT fDXOld;
    static FLOAT fDYOld;
    static TIME tmOldDelta;
    static CTimerValue tvBefore;
    CTimerValue tvNow = _pTimer->GetHighPrecisionTimer();
    TIME tmNowDelta = (tvNow-tvBefore).GetSeconds();
    if (tmNowDelta<0.001f) {
      tmNowDelta = 0.001f;
    }
    tvBefore = tvNow;

    FLOAT fDXSmooth = (fDXOld*tmOldDelta+fDX*tmNowDelta)/(tmOldDelta+tmNowDelta);
    FLOAT fDYSmooth = (fDYOld*tmOldDelta+fDY*tmNowDelta)/(tmOldDelta+tmNowDelta);
    fDXOld = fDX;
    fDYOld = fDY;
    tmOldDelta = tmNowDelta;
    if (inp_bFilterMouse) {
      fDX = fDXSmooth;
      fDY = fDYSmooth;
    }

    // get final mouse values
    FLOAT fMouseRelX = +fDX*fSensitivity;
    FLOAT fMouseRelY = -fDY*fSensitivity;
    if (inp_bInvertMouse) {
      fMouseRelY = -fMouseRelY;
    }
    FLOAT fMouseRelZ = _iMouseZ;

    // just interpret values as normal
    inp_caiAllAxisInfo[1].cai_fReading = fMouseRelX;
    inp_caiAllAxisInfo[2].cai_fReading = fMouseRelY;
    inp_caiAllAxisInfo[3].cai_fReading = fMouseRelZ;
  }

  // if not pre-scanning
  if (!bPreScan) {
    // [Cecil] FIXME: This whole block should be executed at all times and not just when input is needed,
    // otherwise it produces bugs where if you scroll the wheel in the menu a bunch of times, it will:
    // 1. Rapidly switch weapons as soon as you return to the game or...
    // 2. Keep auto-forcing "Mouse Wheel Up" or "Mouse Wheel Dn" to controls on any active key bind
    // until the accumulated _iMouseZ reaches zero
    _bWheelUp = _bWheelDn = FALSE; // [Cecil] TEMP: Reset states to prevent it from toggling each step

    // Detect wheel up/down movement
    if (_iMouseZ > 0) {
      if (_bWheelUp) {
        inp_ubButtonsBuffer[KID_MOUSEWHEELUP] = 0x00;
      } else {
        inp_ubButtonsBuffer[KID_MOUSEWHEELUP] = 0xFF;
        _iMouseZ = ClampDn(_iMouseZ - MOUSEWHEEL_SCROLL_INTERVAL, 0);
      }
    }

    _bWheelUp = inp_ubButtonsBuffer[KID_MOUSEWHEELUP];

    if (_iMouseZ < 0) {
      if (_bWheelDn) {
        inp_ubButtonsBuffer[KID_MOUSEWHEELDOWN] = 0x00;
      } else {
        inp_ubButtonsBuffer[KID_MOUSEWHEELDOWN] = 0xFF;
        _iMouseZ = ClampUp(_iMouseZ + MOUSEWHEEL_SCROLL_INTERVAL, 0);
      }
    }

    _bWheelDn = inp_ubButtonsBuffer[KID_MOUSEWHEELDOWN];
  }

  inp_bLastPrescan = bPreScan;

  // set cursor position to screen center
  if (iMouseX != inp_slScreenCenterX || iMouseY != inp_slScreenCenterY) {
    SetCursorPos(inp_slScreenCenterX, inp_slScreenCenterY);
  }

  // readout 2nd mouse if enabled
  Mouse2_Update(); // [Cecil]

  // if joystick polling is enabled
  if (inp_bPollJoysticks || inp_bForceJoystickPolling) {
    // scan all available joysticks
    for( INDEX iJoy=0; iJoy<MAX_JOYSTICKS; iJoy++) {
      if (inp_abJoystickOn[iJoy] && iJoy<inp_ctJoysticksAllowed) {
        // scan joy state
        BOOL bSucceeded = ScanJoystick(iJoy, bPreScan);
        // if joystick reading failed
        if (!bSucceeded && inp_bAutoDisableJoysticks) {
          // kill it, so it doesn't slow down CPU
          CPrintF(TRANS("Joystick %d failed, disabling it!\n"), iJoy+1);
          inp_abJoystickOn[iJoy] = FALSE;
        }
      }
    }
  }
}

// Clear all input states (keys become not pressed, axes are reset to zero)
void CInput::ClearInput( void)
{
  // clear button's buffer
  memset( inp_ubButtonsBuffer, 0, sizeof( inp_ubButtonsBuffer));
  // clear axis values
  for (INDEX i=0; i<MAX_OVERALL_AXES; i++) {
    inp_caiAllAxisInfo[i].cai_fReading = 0;
  }
}

const CTString &CInput::GetAxisTransName( INDEX iAxisNo) const
{
  return inp_astrAxisTran[iAxisNo];
}
