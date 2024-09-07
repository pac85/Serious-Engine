/* Copyright (c) 2023 Dreamy Cecil
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

#include "OS.h"

#if SE1_WIN
  #include <SDL2/include/SDL_syswm.h>
#endif

// Destroy current window
void OS::Window::Destroy(void) {
  if (pWindow == NULL) return;

#if SE1_PREFER_SDL
  SDL_DestroyWindow(pWindow);
#else
  DestroyWindow(pWindow);
#endif

  pWindow = NULL;
};

#if SE1_WIN

// Retrieve native window handle
HWND OS::Window::GetNativeHandle(void) {
  if (pWindow == NULL) return NULL;

#if SE1_PREFER_SDL
  SDL_SysWMinfo info;

  if (!SDL_GetWindowWMInfo(pWindow, &info)) {
    FatalError(TRANS("Couldn't retrieve driver-specific information about an SDL window:\n%s"), SDL_GetError());
  }

  return info.info.win.window;

#else
  // Already native
  return pWindow;
#endif
};

#endif

void OS::Message::Translate(const MSG *lpMsg)
{
  TranslateMessage(lpMsg);
};

void OS::Message::Dispatch(const MSG *lpMsg)
{
  DispatchMessage(lpMsg);
};

BOOL OS::IsIconic(OS::Window hWnd)
{
#if SE1_PREFER_SDL
  return (hWnd != NULL && (SDL_GetWindowFlags(hWnd) & SDL_WINDOW_MINIMIZED) == SDL_WINDOW_MINIMIZED);
#else
  return ::IsIconic(hWnd);
#endif
};

UWORD OS::GetKeyState(int vKey)
{
  return ::GetKeyState(vKey);
};

UWORD OS::GetAsyncKeyState(int vKey)
{
  return ::GetAsyncKeyState(vKey);
};

BOOL OS::GetCursorPos(int *piX, int *piY, BOOL bRelativeToWindow)
{
#if SE1_PREFER_SDL
  if (bRelativeToWindow) {
    SDL_GetMouseState(piX, piY);
  } else {
    SDL_GetGlobalMouseState(piX, piY);
  }
  return TRUE;

#else
  POINT pt;
  BOOL bResult = ::GetCursorPos(&pt);

  if (bResult && bRelativeToWindow) {
    bResult = ::ScreenToClient(GetActiveWindow(), &pt);
  }

  if (piX != NULL) *piX = pt.x;
  if (piY != NULL) *piY = pt.y;

  return bResult;
#endif
};

int OS::ShowCursor(BOOL bShow)
{
  return ::ShowCursor(bShow);
};
