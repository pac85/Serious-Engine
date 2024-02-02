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

BOOL OS::Message::Peek(MSG *lpMsg, OS::Window hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
  return ::PeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
};

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
  return ::IsIconic(hWnd);
};

UWORD OS::GetKeyState(int vKey)
{
  return ::GetKeyState(vKey);
};

UWORD OS::GetAsyncKeyState(int vKey)
{
  return ::GetAsyncKeyState(vKey);
};

BOOL OS::GetCursorPos(LPPOINT lpPoint)
{
  return ::GetCursorPos(lpPoint);
};

BOOL OS::ScreenToClient(OS::Window hWnd, LPPOINT lpPoint)
{
  return ::ScreenToClient(hWnd, lpPoint);
};

int OS::ShowCursor(BOOL bShow)
{
  return ::ShowCursor(bShow);
};
