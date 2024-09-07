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

BOOL OS::PollEvent(OS::SE1Event &event) {
  // Go in the loop until it finds an event it can process and return TRUE on it
  // Otherwise break from switch-case and try checking the next event
  // If none found, exits the loop and returns FALSE because there are no more events
  MSG msg;

  while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    // If it's not a mouse message
    if (msg.message < WM_MOUSEFIRST || msg.message > WM_MOUSELAST) {
      // And not system key messages
      if (!((msg.message == WM_KEYDOWN && msg.wParam == VK_F10) || msg.message == WM_SYSKEYDOWN)) {
        // Dispatch it
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
      }
    }

    // Reset the event
    SDL_zero(event);
    event.type = msg.message;

    switch (msg.message) {
      // Key events
      case WM_CHAR:
      case WM_SYSKEYDOWN: case WM_SYSKEYUP:
      case WM_KEYDOWN: case WM_KEYUP: {
        event.key.code = msg.wParam;
      } return TRUE;

      // Mouse events
      case WM_MOUSEMOVE: {
        event.mouse.y = HIWORD(msg.lParam);
        event.mouse.x = LOWORD(msg.lParam);
      } return TRUE;

      case WM_MOUSEWHEEL: {
        event.mouse.y = (SWORD)(UWORD)HIWORD(msg.wParam);
      } return TRUE;

      case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK: {
        event.type = WM_LBUTTONDOWN;
        event.mouse.button = SDL_BUTTON_LEFT;
        event.mouse.pressed = TRUE;
      } return TRUE;

      case WM_LBUTTONUP: {
        event.mouse.button = SDL_BUTTON_LEFT;
        event.mouse.pressed = FALSE;
      } return TRUE;

      case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK: {
        event.type = WM_RBUTTONDOWN;
        event.mouse.button = SDL_BUTTON_RIGHT;
        event.mouse.pressed = TRUE;
      } return TRUE;

      case WM_RBUTTONUP: {
        event.mouse.button = SDL_BUTTON_RIGHT;
        event.mouse.pressed = FALSE;
      } return TRUE;

      case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK: {
        event.type = WM_MBUTTONDOWN;
        event.mouse.button = SDL_BUTTON_MIDDLE;
        event.mouse.pressed = TRUE;
      } return TRUE;

      case WM_MBUTTONUP: {
        event.mouse.button = SDL_BUTTON_MIDDLE;
        event.mouse.pressed = FALSE;
      } return TRUE;

      // Window events
      case WM_QUIT: case WM_CLOSE:
        return TRUE;

      case WM_COMMAND: {
        event.window.event = msg.wParam;
        event.window.data = msg.lParam;
      } return TRUE;

      case WM_SYSCOMMAND: {
        switch (msg.wParam & ~0x0F) {
          case SC_MINIMIZE: event.window.event = SDL_WINDOWEVENT_MINIMIZED; break;
          case SC_MAXIMIZE: event.window.event = SDL_WINDOWEVENT_MAXIMIZED; break;
          case SC_RESTORE:  event.window.event = SDL_WINDOWEVENT_RESTORED; break;
          default: event.window.event = SDL_WINDOWEVENT_NONE; // Unknown
        }
      } return TRUE;

      case WM_PAINT: {
        event.type = WM_SYSCOMMAND;
        event.window.event = SDL_WINDOWEVENT_EXPOSED;
      } return TRUE;

      case WM_CANCELMODE: {
        event.type = WM_SYSCOMMAND;
        event.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
      } return TRUE;

      case WM_KILLFOCUS: {
        event.type = WM_SYSCOMMAND;
        event.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
      } return TRUE;

      case WM_SETFOCUS: {
        event.type = WM_SYSCOMMAND;
        event.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
      } return TRUE;

      case WM_ACTIVATE: {
        switch (LOWORD(msg.wParam))
        {
          case WA_ACTIVE: case WA_CLICKACTIVE: {
            event.type = WM_SYSCOMMAND;
            event.window.event = SDL_WINDOWEVENT_ENTER;
          } return TRUE;

          case WA_INACTIVE: {
            event.type = WM_SYSCOMMAND;
            event.window.event = SDL_WINDOWEVENT_LEAVE;
          } return TRUE;

          default:
            // Minimized
            if (HIWORD(msg.wParam)) {
              // Deactivated
              event.type = WM_SYSCOMMAND;
              event.window.event = SDL_WINDOWEVENT_LEAVE;
              return TRUE;
            }
        }
      } break;

      case WM_ACTIVATEAPP: {
        if (msg.wParam) {
          event.type = WM_SYSCOMMAND;
          event.window.event = SDL_WINDOWEVENT_ENTER;
        } else {
          event.type = WM_SYSCOMMAND;
          event.window.event = SDL_WINDOWEVENT_LEAVE;
        }
      } return TRUE;

      default: break;
    }
  }

  return FALSE;
};

BOOL OS::IsIconic(OS::Window hWnd)
{
#if SE1_PREFER_SDL
  return (hWnd != NULL && !!(SDL_GetWindowFlags(hWnd) & SDL_WINDOW_MINIMIZED));
#else
  return ::IsIconic(hWnd);
#endif
};

UWORD OS::GetKeyState(ULONG iKey)
{
  return ::GetAsyncKeyState(iKey);
};

ULONG OS::GetMouseState(int *piX, int *piY, BOOL bRelativeToWindow) {
  ULONG ulMouse = 0;

#if SE1_PREFER_SDL
  if (bRelativeToWindow) {
    ulMouse = SDL_GetMouseState(piX, piY);
  } else {
    ulMouse = SDL_GetGlobalMouseState(piX, piY);
  }

#else
  POINT pt;
  BOOL bResult = ::GetCursorPos(&pt);

  // Can't afford to handle errors
  if (!bResult) {
    pt.x = pt.y = 0;

  } else if (bRelativeToWindow) {
    bResult = ::ScreenToClient(GetActiveWindow(), &pt);
  }

  if (piX != NULL) *piX = pt.x;
  if (piY != NULL) *piY = pt.y;

  // Gather mouse states
  if (::GetKeyState(VK_LBUTTON) & 0x8000) ulMouse |= SDL_BUTTON_LMASK;
  if (::GetKeyState(VK_RBUTTON) & 0x8000) ulMouse |= SDL_BUTTON_RMASK;
  if (::GetKeyState(VK_MBUTTON) & 0x8000) ulMouse |= SDL_BUTTON_MMASK;
#endif

  return ulMouse;
};

int OS::ShowCursor(BOOL bShow)
{
  return ::ShowCursor(bShow);
};
