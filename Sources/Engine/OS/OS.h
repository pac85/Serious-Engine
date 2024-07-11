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

// [Cecil] This header defines a cross-platform interface for interacting with operating systems
#ifndef SE_INCL_OS_H
#define SE_INCL_OS_H

// Compatibility with multiple platforms
#include <Engine/OS/PlatformSpecific.h>

// Non-Windows OS
#if !SE1_WIN

#define LOWORD(x) (x & 0xFFFF)
#define HIWORD(x) ((x >> 16) & 0xFFFF)

#endif // !SE1_WIN

class ENGINE_API OS {
  public:
    // Handle pointers
  #if SE1_PREFER_SDL
    typedef SDL_Window *WndHandle; // Window handle
    typedef WndHandle DvcContext;  // Alias for compatibility
  #else
    typedef HWND WndHandle; // Window handle
    typedef HDC DvcContext; // Device context handle
  #endif

    // Window handler
    struct ENGINE_API Window {
      WndHandle pWindow;

      Window(int i = NULL) : pWindow((WndHandle)(size_t)i) {};
      Window(size_t i) : pWindow((WndHandle)i) {};
    #if !SE1_WIN
      Window(long int i) : pWindow((WndHandle)i) {};
    #endif

      Window(const Window &other) : pWindow(other.pWindow) {};
      Window(const WndHandle pSetWindow) : pWindow(pSetWindow) {};

      operator WndHandle() { return pWindow; };
      operator const WndHandle() const { return pWindow; };

      inline bool operator==(const Window &other) const { return pWindow == other.pWindow; };
      inline bool operator!=(const Window &other) const { return pWindow != other.pWindow; };
      inline bool operator==(size_t i) const { return pWindow == (WndHandle)i; };
      inline bool operator!=(size_t i) const { return pWindow != (WndHandle)i; };

      // Destroy current window
      void Destroy(void);
    };

    // Depending on build configuration this structure can either be:
    // - A handle to a dynamic module
    // - A dummy for a static module
    struct EngineModule {
      HMODULE hHandle;

      EngineModule(HMODULE hOther = NULL) : hHandle(hOther) {};

    #if defined(SE1_STATIC_BUILD)
      inline BOOL IsLoaded(void) { return TRUE; };
      inline void Load(const char *strLibrary) {};
      inline void LoadOrThrow_t(const char *strLibrary) {};
      inline BOOL Free(void) { return TRUE; };

    #else
      inline BOOL IsLoaded(void) { return hHandle != NULL; };
      inline void Load(const char *strLibrary) { hHandle = LoadLib(strLibrary); };
      inline void LoadOrThrow_t(const char *strLibrary) { hHandle = LoadLibOrThrow_t(strLibrary); };

      inline BOOL Free(void) {
        if (!IsLoaded()) return FALSE;
        BOOL bReturn = FreeLib(hHandle);
        hHandle = NULL;
        return bReturn;
      };
    #endif // SE1_STATIC_BUILD

      inline EngineModule &operator=(const EngineModule &hOther) {
        hHandle = hOther.hHandle;
        return *this;
      };
    };

  // Dynamic library methods
  public:

    // Load library
    static HMODULE LoadLib(const char *strLibrary);

    // Throw an error if unable to load a library
    static HMODULE LoadLibOrThrow_t(const char *strLibrary);

    // Free loaded library
    static BOOL FreeLib(HMODULE hLib);

    // Hook library symbol
    static void *GetLibSymbol(HMODULE hLib, const char *strSymbol);

  // Cross-platform reimplementations of methods from Windows API
  public:

    struct ENGINE_API Message {
      // PeekMessage()
      static BOOL Peek(MSG *lpMsg, Window hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);

      // TranslateMessage()
      static void Translate(const MSG *lpMsg);

      // DispatchMessage()
      static void Dispatch(const MSG *lpMsg);
    };

    static BOOL IsIconic(Window hWnd);
    static UWORD GetKeyState(int vKey);
    static UWORD GetAsyncKeyState(int vKey);
    static BOOL GetCursorPos(LPPOINT lpPoint);
    static BOOL ScreenToClient(Window hWnd, LPPOINT lpPoint);
    static int ShowCursor(BOOL bShow);
};

#endif // include-once check
