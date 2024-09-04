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

#include <Engine/Base/Input.h>

extern INDEX inp_ctJoysticksAllowed;
extern CTString inp_astrAxisTran[MAX_OVERALL_AXES];

// [Cecil] Set names for joystick axes and buttons in a separate method
void CInput::SetJoystickNames(void) {
  // Get number of joystics
  INDEX ctJoysticksPresent = joyGetNumDevs();
  CPrintF(TRANS("  joysticks found: %d\n"), ctJoysticksPresent);

  ctJoysticksPresent = Min(ctJoysticksPresent, inp_ctJoysticksAllowed);
  CPrintF(TRANS("  joysticks allowed: %d\n"), ctJoysticksPresent);

  // Enumerate axis and buttons for joysticks
  for (INDEX iJoy = 0; iJoy < MAX_JOYSTICKS; iJoy++) {
    inp_abJoystickOn[iJoy] = FALSE;

    if (iJoy < ctJoysticksPresent && CheckJoystick(iJoy)) {
      inp_abJoystickOn[iJoy] = TRUE;
    }

    AddJoystickAbbilities(iJoy);
  }
};

void CInput::SetJoyPolling(BOOL bPoll) {
  inp_bPollJoysticks = bPoll;
};

// Check if a joystick exists
BOOL CInput::CheckJoystick(INDEX iJoy) {
  CPrintF(TRANS("  joy %d:"), iJoy + 1);

  // Seek for capabilities of a requested joystick
  JOYCAPS jc;
  MMRESULT mmResult = joyGetDevCaps(JOYSTICKID1 + iJoy, &jc, sizeof(JOYCAPS));

  // Report possible errors
  if (mmResult == MMSYSERR_NODRIVER) {
    CPrintF(TRANS(" joystick driver is not present\n"));
    return FALSE;

  } else if (mmResult == MMSYSERR_INVALPARAM) {
    CPrintF(TRANS(" invalid parameter\n"));
    return FALSE;

  } else if (mmResult != JOYERR_NOERROR) {
    CPrintF(TRANS("  error 0x%08x\n"), mmResult);
    return FALSE;
  }

  CPrintF(" '%s'\n", jc.szPname);

  CPrintF(TRANS("    %d axes\n"), jc.wNumAxes);
  CPrintF(TRANS("    %d buttons\n"), jc.wNumButtons);

  if (jc.wCaps & JOYCAPS_HASPOV) {
    CPrintF(TRANS("    POV hat present\n"));
    inp_abJoystickHasPOV[iJoy] = TRUE;
  } else {
    inp_abJoystickHasPOV[iJoy] = FALSE;
  }

  // Read joystick state
  JOYINFOEX ji;
  ji.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNCENTERED | JOY_RETURNPOV | JOY_RETURNR
    | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNU | JOY_RETURNV;
  ji.dwSize = sizeof(JOYINFOEX);

  mmResult = joyGetPosEx(JOYSTICKID1 + iJoy, &ji);

  // Some error occurred
  if (mmResult != JOYERR_NOERROR) {
    CPrintF(TRANS("    Cannot read the joystick!\n"));
    return FALSE;
  }

  // [Cecil] Replaced useless loop with a series of statements
  ASSERT(MAX_AXES_PER_JOYSTICK == 6);
  const INDEX iFirstJoyAxis = FIRST_JOYAXIS + iJoy * MAX_AXES_PER_JOYSTICK;

  // Remember min/max info for each joystick axis
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 0];
    cai.cai_slMin = jc.wXmin;
    cai.cai_slMax = jc.wXmax;
    cai.cai_bExisting = TRUE;
  }
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 1];
    cai.cai_slMin = jc.wYmin;
    cai.cai_slMax = jc.wYmax;
    cai.cai_bExisting = TRUE;
  }
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 2];
    cai.cai_slMin = jc.wZmin;
    cai.cai_slMax = jc.wZmax;
    cai.cai_bExisting = !!(jc.wCaps & JOYCAPS_HASZ);
  }
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 3];
    cai.cai_slMin = jc.wRmin;
    cai.cai_slMax = jc.wRmax;
    cai.cai_bExisting = !!(jc.wCaps & JOYCAPS_HASR);
  }
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 4];
    cai.cai_slMin = jc.wUmin;
    cai.cai_slMax = jc.wUmax;
    cai.cai_bExisting = !!(jc.wCaps & JOYCAPS_HASU);
  }
  {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[iFirstJoyAxis + 5];
    cai.cai_slMin = jc.wVmin;
    cai.cai_slMax = jc.wVmax;
    cai.cai_bExisting = !!(jc.wCaps & JOYCAPS_HASV);
  }

  return TRUE;
};

// Adds axis and buttons for given joystick
void CInput::AddJoystickAbbilities(INDEX iJoy) {
  const CTString strJoystickName(0, "Joy %d", iJoy + 1);
  const CTString strJoystickNameTra(0, TRANS("Joy %d"), iJoy + 1);

  // Set names for all axes
  for (INDEX iAxis = 0; iAxis < MAX_AXES_PER_JOYSTICK; iAxis++)
  {
    const INDEX iAxisTotal = FIRST_JOYAXIS + iJoy * MAX_AXES_PER_JOYSTICK + iAxis;
    ControlAxisInfo &cai= inp_caiAllAxisInfo[iAxisTotal];

    CTString &str = cai.cai_strAxisName;
    str = strJoystickName;

    CTString &strTran = inp_astrAxisTran[iAxisTotal];
    strTran = strJoystickNameTra;

    switch (iAxis) {
      case 0: str += " Axis X"; strTran += TRANS(" Axis X"); break;
      case 1: str += " Axis Y"; strTran += TRANS(" Axis Y"); break;
      case 2: str += " Axis Z"; strTran += TRANS(" Axis Z"); break;
      case 3: str += " Axis R"; strTran += TRANS(" Axis R"); break;
      case 4: str += " Axis U"; strTran += TRANS(" Axis U"); break;
      case 5: str += " Axis V"; strTran += TRANS(" Axis V"); break;
    }
  }

  INDEX iButtonTotal = FIRST_JOYBUTTON + iJoy * MAX_BUTTONS_PER_JOYSTICK;

  // [Cecil] For convenience
  #define SET_BUTTON_NAME(_Name, _Translated) \
    inp_strButtonNames   [iButtonTotal] = strJoystickName    + _Name; \
    inp_strButtonNamesTra[iButtonTotal] = strJoystickNameTra + _Translated; \
    iButtonTotal++;

  // Set names to buttons that the joystick supports
  for (INDEX iButton = 0; iButton < MAX_BUTTONS_PER_JOYSTICK_NOPOV; iButton++)
  {
    CTString strButtonName(0, " Button %d", iButton);
    CTString strButtonNameTra(0, TRANS(" Button %d"), iButton);

    SET_BUTTON_NAME(strButtonName, strButtonNameTra);
  }

  // Add the four POV buttons
  SET_BUTTON_NAME(" POV N", TRANS(" POV N"));
  SET_BUTTON_NAME(" POV E", TRANS(" POV E"));
  SET_BUTTON_NAME(" POV S", TRANS(" POV S"));
  SET_BUTTON_NAME(" POV W", TRANS(" POV W"));

  #undef SET_BUTTON_NAME
};

// Scans axis and buttons for given joystick
BOOL CInput::ScanJoystick(INDEX iJoy, BOOL bPreScan) {
  // Read joystick state
  JOYINFOEX ji;
  ji.dwFlags = JOY_RETURNBUTTONS | JOY_RETURNCENTERED | JOY_RETURNPOV | JOY_RETURNR
    | JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNU | JOY_RETURNV;
  ji.dwSize = sizeof(JOYINFOEX);

  MMRESULT mmResult = joyGetPosEx(JOYSTICKID1 + iJoy, &ji);

  // Some error occurred
  if (mmResult != JOYERR_NOERROR) return FALSE;

  // For each available axis
  for (INDEX iAxis = 0; iAxis < MAX_AXES_PER_JOYSTICK; iAxis++) {
    ControlAxisInfo &cai = inp_caiAllAxisInfo[FIRST_JOYAXIS + iJoy * MAX_AXES_PER_JOYSTICK + iAxis];

    // If the axis is not present
    if (!cai.cai_bExisting) {
      // Read as zero and skip to the next one
      cai.cai_fReading = 0.0f;
      continue;
    }

    // Read its state
    SLONG slAxisReading;

    switch (iAxis) {
      case 0: slAxisReading = ji.dwXpos; break;
      case 1: slAxisReading = ji.dwYpos; break;
      case 2: slAxisReading = ji.dwZpos; break;
      case 3: slAxisReading = ji.dwRpos; break;
      case 4: slAxisReading = ji.dwUpos; break;
      case 5: slAxisReading = ji.dwVpos; break;
      default: ASSERT(FALSE);
    }

    // Convert from min..max to -1..+1
    FLOAT fAxisReading = FLOAT(slAxisReading - cai.cai_slMin) / FLOAT(cai.cai_slMax - cai.cai_slMin) * 2.0f - 1.0f;

    // Set current axis value
    cai.cai_fReading = fAxisReading;
  }

  // If not pre-scanning
  if (!bPreScan) {
    INDEX iButtonTotal = FIRST_JOYBUTTON + iJoy * MAX_BUTTONS_PER_JOYSTICK;

    // For each available button
    for (INDEX iButton = 0; iButton < MAX_BUTTONS_PER_JOYSTICK_NOPOV; iButton++) {
      // Test if the button is pressed
      if (ji.dwButtons & (1L << iButton)) {
        inp_ubButtonsBuffer[iButtonTotal++] = 128;
      } else {
        inp_ubButtonsBuffer[iButtonTotal++] = 0;
      }
    }

    // POV hat initially not pressed
    //CPrintF("%d\n", ji.dwPOV);
    INDEX iStartPOV = iButtonTotal;
    inp_ubButtonsBuffer[iStartPOV + 0] = 0;
    inp_ubButtonsBuffer[iStartPOV + 1] = 0;
    inp_ubButtonsBuffer[iStartPOV + 2] = 0;
    inp_ubButtonsBuffer[iStartPOV + 3] = 0;

    // If we have POV
    if (inp_abJoystickHasPOV[iJoy])
    {
      // Check the four POV directions
      if (ji.dwPOV == JOY_POVFORWARD) {
        inp_ubButtonsBuffer[iStartPOV + 0] = 128;

      } else if (ji.dwPOV == JOY_POVRIGHT) {
        inp_ubButtonsBuffer[iStartPOV + 1] = 128;

      } else if (ji.dwPOV == JOY_POVBACKWARD) {
        inp_ubButtonsBuffer[iStartPOV + 2] = 128;

      } else if (ji.dwPOV == JOY_POVLEFT) {
        inp_ubButtonsBuffer[iStartPOV + 3] = 128;

      // And four mid-positions
      } else if (ji.dwPOV == JOY_POVFORWARD + 4500) {
        inp_ubButtonsBuffer[iStartPOV + 0] = 128;
        inp_ubButtonsBuffer[iStartPOV + 1] = 128;

      } else if (ji.dwPOV == JOY_POVRIGHT + 4500) {
        inp_ubButtonsBuffer[iStartPOV + 1] = 128;
        inp_ubButtonsBuffer[iStartPOV + 2] = 128;

      } else if (ji.dwPOV == JOY_POVBACKWARD + 4500) {
        inp_ubButtonsBuffer[iStartPOV + 2] = 128;
        inp_ubButtonsBuffer[iStartPOV + 3] = 128;

      } else if (ji.dwPOV == JOY_POVLEFT + 4500) {
        inp_ubButtonsBuffer[iStartPOV + 3] = 128;
        inp_ubButtonsBuffer[iStartPOV + 0] = 128;
      }
    }
  }

  return TRUE;
};
