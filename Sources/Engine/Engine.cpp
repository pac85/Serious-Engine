/* Copyright (c) 2002-2012 Croteam Ltd. 
   Copyright (c) 2023 Dreamy Cecil
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

#include <Engine/Build.h>
#include <Engine/Base/Profiling.h>
#include <Engine/Base/Input.h>
#include <Engine/Base/Protection.h>
#include <Engine/Base/Console.h>
#include <Engine/Base/Console_internal.h>
#include <Engine/Base/Statistics_internal.h>
#include <Engine/Base/Shell.h>
#include <Engine/Base/CRC.h>
#include <Engine/Base/CRCTable.h>
#include <Engine/Base/ProgressHook.h>
#include <Engine/Sound/SoundListener.h>
#include <Engine/Sound/SoundLibrary.h>
#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Font.h>
#include <Engine/Network/Network.h>
#include <Engine/Templates/DynamicContainer.cpp>
#include <Engine/Templates/Stock_CAnimData.h>
#include <Engine/Templates/Stock_CTextureData.h>
#include <Engine/Templates/Stock_CSoundData.h>
#include <Engine/Templates/Stock_CModelData.h>
#include <Engine/Templates/Stock_CEntityClass.h>
#include <Engine/Templates/Stock_CMesh.h>
#include <Engine/Templates/Stock_CSkeleton.h>
#include <Engine/Templates/Stock_CAnimSet.h>
#include <Engine/Templates/Stock_CShader.h>
#include <Engine/Templates/StaticArray.cpp>
#include <Engine/Base/IFeel.h>

// this version string can be referenced from outside the engine
ENGINE_API CTString _strEngineBuild  = "";
ENGINE_API ULONG _ulEngineBuildMajor = _SE_BUILD_MAJOR;
ENGINE_API ULONG _ulEngineBuildMinor = _SE_BUILD_MINOR;

// [Cecil] TEMP: Current application type
EEngineAppType _eEngineAppType = E_SEAPP_OTHER;

ENGINE_API CTString _strLogFile = "";

// global handle for application window
OS::Window _hwndCurrent = NULL;
BOOL _bFullScreen = FALSE;

// critical section for access to zlib functions
CTCriticalSection zip_csLock; 

#if !SE1_PREFER_SDL
// to keep system gamma table
static UWORD auwSystemGamma[256*3];
#endif


// OS info
static CTString sys_strOS = "";
static INDEX sys_iOSMajor = 0;
static INDEX sys_iOSMinor = 0;
static INDEX sys_iOSBuild = 0;
static CTString sys_strOSMisc = "";

// CPU info
static CTString sys_strCPUVendor = "";
static INDEX sys_iCPUType = 0;
static INDEX sys_iCPUFamily = 0;
static INDEX sys_iCPUModel = 0;
static INDEX sys_iCPUStepping = 0;
static BOOL  sys_bCPUHasMMX = 0;
static BOOL  sys_bCPUHasCMOV = 0;
static INDEX sys_iCPUMHz = 0;
       INDEX sys_iCPUMisc = 0;

// RAM info
static INDEX sys_iRAMPhys = 0;
static INDEX sys_iRAMSwap = 0;

// HDD info
static INDEX sys_iHDDSize = 0;
static INDEX sys_iHDDFree = 0;
static INDEX sys_iHDDMisc = 0;

// MOD info
static CTString sys_strModName = "";
static CTString sys_strModExt  = "";

// enables paranoia checks for allocation array
BOOL _bAllocationArrayParanoiaCheck = FALSE;

static void DetectCPU(void)
{
  char strVendor[12+1];
  strVendor[12] = 0;
  ULONG ulTFMS;
  ULONG ulFeatures;

  // test MMX presence and update flag
#if SE1_OLD_COMPILER || SE1_USE_ASM
  __asm {
    mov     eax,0           ;// request for basic id
    cpuid
    mov     dword ptr [strVendor+0], ebx
    mov     dword ptr [strVendor+4], edx
    mov     dword ptr [strVendor+8], ecx
    mov     eax,1           ;// request for TFMS feature flags
    cpuid
    mov     dword ptr [ulTFMS], eax ;// remember type, family, model and stepping
    mov     dword ptr [ulFeatures], edx
  }

#else
  union {
    int regs[4];

    struct {
      int EAX, EBX, ECX, EDX;
    };
  } procID;

  // Get highest function parameter and CPU's manufacturer ID string
  __cpuid(procID.regs, 0);

  memcpy(&strVendor[0], &procID.EBX, 4);
  memcpy(&strVendor[4], &procID.EDX, 4);
  memcpy(&strVendor[8], &procID.ECX, 4);

  // Get processor info and feature bits
  __cpuid(procID.regs, 1);

  memcpy(&ulTFMS, &procID.EAX, 4);
  memcpy(&ulFeatures, &procID.EDX, 4);
#endif

  INDEX iType     = (ulTFMS>>12)&0x3;
  INDEX iFamily   = (ulTFMS>> 8)&0xF;
  INDEX iModel    = (ulTFMS>> 4)&0xF;
  INDEX iStepping = (ulTFMS>> 0)&0xF;


  CPrintF(TRANS("  Vendor: %s\n"), strVendor);
  CPrintF(TRANS("  Type: %d, Family: %d, Model: %d, Stepping: %d\n"),
    iType, iFamily, iModel, iStepping);

  BOOL bMMX  = ulFeatures & (1<<23);
  BOOL bCMOV = ulFeatures & (1<<15);

  const char *strYes = TRANS("Yes");
  const char *strNo = TRANS("No");

  CPrintF(TRANS("  MMX : %s\n"), bMMX ?strYes:strNo);
  CPrintF(TRANS("  CMOV: %s\n"), bCMOV?strYes:strNo);
  CPrintF(TRANS("  Clock: %.0fMHz\n"), _pTimer->tm_llCPUSpeedHZ/1E6);

  sys_strCPUVendor = strVendor;
  sys_iCPUType = iType;
  sys_iCPUFamily =  iFamily;
  sys_iCPUModel = iModel;
  sys_iCPUStepping = iStepping;
  sys_bCPUHasMMX = bMMX!=0;
  sys_bCPUHasCMOV = bCMOV!=0;
  sys_iCPUMHz = INDEX(_pTimer->tm_llCPUSpeedHZ/1E6);

  if( !bMMX) FatalError( TRANS("MMX support required but not present!"));
}

static void DetectCPUWrapper(void)
{
  __try {
    DetectCPU();
  } __except(EXCEPTION_EXECUTE_HANDLER) {
    CPrintF( TRANS("Cannot detect CPU: exception raised.\n"));
  }
}

// [Cecil] SDL: Initialization and shutdown management
static bool _bEngineInitializedSDL = false;

static void SE_EndSDL(void) {
  // Already shut down
  if (!_bEngineInitializedSDL) return;

  SDL_Quit();
  _bEngineInitializedSDL = false;
};

static void SE_InitSDL(ULONG ulFlags) {
  // Already initialized
  if (_bEngineInitializedSDL) return;

  // Main initialization (may be basic with 0 flags)
  ULONG ulNoJoysticks = ulFlags & ~SDL_INIT_JOYSTICK;

  if (SDL_Init(ulNoJoysticks) == -1) {
    FatalError(TRANS("SDL_Init(0x%X) failed:\n%s"), ulNoJoysticks, SDL_GetError());
  }

  // Optional
  if (ulFlags & SDL_INIT_JOYSTICK) {
    if (SDL_Init(SDL_INIT_JOYSTICK) == -1) {
      CPrintF(TRANS("SDL_Init(SDL_INIT_JOYSTICK) failed:\n%s\n"), SDL_GetError());
    }
  }

  atexit(SE_EndSDL);
  _bEngineInitializedSDL = true;
};

// startup engine 
ENGINE_API void SE_InitEngine(EEngineAppType eType)
{
  // [Cecil] SDL: Initialize for gameplay or for basic stuff
  const BOOL bGameApp = (eType == E_SEAPP_GAME || eType == E_SEAPP_EDITOR);
  const ULONG ulGameplay = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK;

  SE_InitSDL(bGameApp ? ulGameplay : 0);

  // [Cecil] TEMP: Set application type
  _eEngineAppType = eType;

  // [Cecil] Determine application paths for the first time
  DetermineAppPaths();

  _pConsole = new CConsole;
  if (_strLogFile=="") {
    _strLogFile = _fnmApplicationExe.FileName();
  }

  // [Cecil] Save under the "Logs/" directory
  CreateAllDirectories("Logs\\");
  _pConsole->Initialize(_fnmApplicationPath + "Logs\\" + _strLogFile + ".log", 90, 512);

  _pAnimStock        = new CStock_CAnimData;
  _pTextureStock     = new CStock_CTextureData;
  _pSoundStock       = new CStock_CSoundData;
  _pModelStock       = new CStock_CModelData;
  _pEntityClassStock = new CStock_CEntityClass;
  _pMeshStock        = new CStock_CMesh;
  _pSkeletonStock    = new CStock_CSkeleton;
  _pAnimSetStock     = new CStock_CAnimSet;
  _pShaderStock      = new CStock_CShader;

  // init main shell
  _pShell = new CShell;
  _pShell->Initialize();

  _pTimer = new CTimer;
  _pGfx   = new CGfxLibrary;
  _pSound = new CSoundLibrary;
  _pInput = new CInput;
  _pNetwork = new CNetworkLibrary;

  CRCT_Init();

  _strEngineBuild.PrintF( TRANS("SeriousEngine Build: %d.%d"), _SE_BUILD_MAJOR, _SE_BUILD_MINOR);

  // print basic engine info
  CPrintF(TRANS("--- Serious Engine Startup ---\n"));
  CPrintF("  %s\n\n", _strEngineBuild.ConstData());

  // print info on the started application
  CPrintF(TRANS("Executable: %s\n"), (_fnmApplicationPath + _fnmApplicationExe).ConstData());
  CPrintF(TRANS("Assumed engine directory: %s\n"), _fnmApplicationPath.ConstData());

  CPutString("\n");

  // report os info
  CPutString(TRANS("Examining underlying OS...\n"));
  OSVERSIONINFOA osv;
  memset(&osv, 0, sizeof(osv));
  osv.dwOSVersionInfoSize = sizeof(osv);
  if (GetVersionExA(&osv)) {
    switch (osv.dwPlatformId) {
    case VER_PLATFORM_WIN32s:         sys_strOS = "Win32s";  break;
    case VER_PLATFORM_WIN32_WINDOWS:  sys_strOS = "Win9x"; break;
    case VER_PLATFORM_WIN32_NT:       sys_strOS = "WinNT"; break;
    default: sys_strOS = "Unknown\n"; break;
    }

    sys_iOSMajor = osv.dwMajorVersion;
    sys_iOSMinor = osv.dwMinorVersion;
    sys_iOSBuild = osv.dwBuildNumber & 0xFFFF;
    sys_strOSMisc = osv.szCSDVersion;

    CPrintF(TRANS("  Type: %s\n"), sys_strOS.ConstData());
    CPrintF(TRANS("  Version: %d.%d, build %d\n"), 
      osv.dwMajorVersion, osv.dwMinorVersion, osv.dwBuildNumber & 0xFFFF);
    CPrintF(TRANS("  Misc: %s\n"), osv.szCSDVersion);
  } else {
    CPrintF(TRANS("Error getting OS info: %s\n"), GetWindowsError(GetLastError()).ConstData());
  }
  CPutString("\n");

  // report CPU
  CPutString(TRANS("Detecting CPU...\n"));
  DetectCPUWrapper();
  CPutString("\n");

  // report memory info
  extern void ReportGlobalMemoryStatus(void);
  ReportGlobalMemoryStatus();

  MEMORYSTATUS ms;
  GlobalMemoryStatus(&ms);

  #define MB (1024*1024)
  sys_iRAMPhys = ms.dwTotalPhys    /MB;
  sys_iRAMSwap = ms.dwTotalPageFile/MB;

  // initialize zip semaphore
  zip_csLock.cs_iIndex = -1;  // not checked for locking order


  // get info on the first disk in system
  DWORD dwSerial;
  DWORD dwFreeClusters;
  DWORD dwClusters;
  DWORD dwSectors;
  DWORD dwBytes;

  char strDrive[] = "C:\\";
  strDrive[0] = _fnmApplicationPath[0];

  GetVolumeInformationA(strDrive, NULL, 0, &dwSerial, NULL, NULL, NULL, 0);
  GetDiskFreeSpaceA(strDrive, &dwSectors, &dwBytes, &dwFreeClusters, &dwClusters);
  sys_iHDDSize = SQUAD(dwSectors)*dwBytes*dwClusters/MB;
  sys_iHDDFree = SQUAD(dwSectors)*dwBytes*dwFreeClusters/MB;
  sys_iHDDMisc = dwSerial;
 
  // add console variables
  extern INDEX con_bNoWarnings;
  extern INDEX wld_bFastObjectOptimization;
  extern INDEX fil_bPreferZips;
  extern FLOAT mth_fCSGEpsilon;
  _pShell->DeclareSymbol("user INDEX con_bNoWarnings;", &con_bNoWarnings);
  _pShell->DeclareSymbol("user INDEX wld_bFastObjectOptimization;", &wld_bFastObjectOptimization);
  _pShell->DeclareSymbol("user FLOAT mth_fCSGEpsilon;", &mth_fCSGEpsilon);
  _pShell->DeclareSymbol("persistent user INDEX fil_bPreferZips;", &fil_bPreferZips);
  // OS info
  _pShell->DeclareSymbol("user const CTString sys_strOS    ;", &sys_strOS);
  _pShell->DeclareSymbol("user const INDEX sys_iOSMajor    ;", &sys_iOSMajor);
  _pShell->DeclareSymbol("user const INDEX sys_iOSMinor    ;", &sys_iOSMinor);
  _pShell->DeclareSymbol("user const INDEX sys_iOSBuild    ;", &sys_iOSBuild);
  _pShell->DeclareSymbol("user const CTString sys_strOSMisc;", &sys_strOSMisc);
  // CPU info
  _pShell->DeclareSymbol("user const CTString sys_strCPUVendor;", &sys_strCPUVendor);
  _pShell->DeclareSymbol("user const INDEX sys_iCPUType       ;", &sys_iCPUType    );
  _pShell->DeclareSymbol("user const INDEX sys_iCPUFamily     ;", &sys_iCPUFamily  );
  _pShell->DeclareSymbol("user const INDEX sys_iCPUModel      ;", &sys_iCPUModel   );
  _pShell->DeclareSymbol("user const INDEX sys_iCPUStepping   ;", &sys_iCPUStepping);
  _pShell->DeclareSymbol("user const INDEX sys_bCPUHasMMX     ;", &sys_bCPUHasMMX  );
  _pShell->DeclareSymbol("user const INDEX sys_bCPUHasCMOV    ;", &sys_bCPUHasCMOV );
  _pShell->DeclareSymbol("user const INDEX sys_iCPUMHz        ;", &sys_iCPUMHz     );
  _pShell->DeclareSymbol("     const INDEX sys_iCPUMisc       ;", &sys_iCPUMisc    );
  // RAM info
  _pShell->DeclareSymbol("user const INDEX sys_iRAMPhys;", &sys_iRAMPhys);
  _pShell->DeclareSymbol("user const INDEX sys_iRAMSwap;", &sys_iRAMSwap);
  _pShell->DeclareSymbol("user const INDEX sys_iHDDSize;", &sys_iHDDSize);
  _pShell->DeclareSymbol("user const INDEX sys_iHDDFree;", &sys_iHDDFree);
  _pShell->DeclareSymbol("     const INDEX sys_iHDDMisc;", &sys_iHDDMisc);
  // MOD info
  _pShell->DeclareSymbol("user const CTString sys_strModName;", &sys_strModName);
  _pShell->DeclareSymbol("user const CTString sys_strModExt;",  &sys_strModExt);

  // Stock clearing
  extern void FreeUnusedStock(void);
  _pShell->DeclareSymbol("user void FreeUnusedStock(void);", &FreeUnusedStock);
  
  // Timer tick quantum
  _pShell->DeclareSymbol("user const FLOAT fTickQuantum;", (FLOAT*)&_pTimer->TickQuantum);

  // init MODs and stuff ...
  extern void InitStreams(void);
  InitStreams();
  // keep mod name in sys cvar
  sys_strModName = _strModName;
  sys_strModExt  = _strModExt;

// checking of crc
#if 0
  ULONG ulCRCActual = -2;
  SLONG ulCRCExpected = -1;
  try {
    // get the checksum of engine
    #ifndef NDEBUG
      #define SELFFILE "Bin\\Debug\\EngineD.dll"
      #define SELFCRCFILE "Bin\\Debug\\EngineD.crc"
    #else
      #define SELFFILE "Bin\\Engine.dll"
      #define SELFCRCFILE "Bin\\Engine.crc"
    #endif
    ulCRCActual = GetFileCRC32_t(CTString(SELFFILE));
    // load expected checksum from the file on disk
    ulCRCExpected = 0;
    LoadIntVar(CTString(SELFCRCFILE), ulCRCExpected);
  } catch (char *strError) {
    CPrintF("%s\n", strError);
  }
  // if not same
  if (ulCRCActual!=ulCRCExpected) {
    // don't run
    //FatalError(TRANS("Engine CRC is invalid.\nExpected %08x, but found %08x.\n"), ulCRCExpected, ulCRCActual);
  }
#endif

  _pInput->Initialize();

  _pGfx->Init();
  _pSound->Init();

  // [Cecil] TEMP: Significant application
  if (eType != E_SEAPP_OTHER) {
    _pNetwork->Init();
    // just make classes declare their shell variables
    try {
      CEntityClass* pec = _pEntityClassStock->Obtain_t(CTString("Classes\\Player.ecl"));
      ASSERT(pec != NULL);
      _pEntityClassStock->Release(pec);  // this must not be a dependency!
    // if couldn't find player class
    } catch (char *strError) {
      FatalError(TRANS("Cannot initialize classes:\n%s"), strError);
    }
  } else {
    _pNetwork = NULL;
  }

  // mark that default fonts aren't loaded (yet)
  _pfdDisplayFont = NULL;
  _pfdConsoleFont = NULL;

#if !SE1_PREFER_SDL
  // readout system gamma table
  HDC  hdc = GetDC(NULL);
  BOOL bOK = GetDeviceGammaRamp( hdc, &auwSystemGamma[0]);
  _pGfx->gl_ulFlags |= GLF_ADJUSTABLEGAMMA;
  if( !bOK) {
    _pGfx->gl_ulFlags &= ~GLF_ADJUSTABLEGAMMA;
    CPutString( TRANS("\nWARNING: Gamma, brightness and contrast are not adjustable!\n\n"));
  } // done
  ReleaseDC( NULL, hdc);
#endif

  // [Cecil] Initialize IFeel
  SE_IFeelInit();
}


// shutdown entire engine
ENGINE_API void SE_EndEngine(void)
{
#if !SE1_PREFER_SDL
  // restore system gamma table (if needed)
  if( _pGfx->gl_ulFlags&GLF_ADJUSTABLEGAMMA) {
    HDC  hdc = GetDC(NULL);
    BOOL bOK = SetDeviceGammaRamp( hdc, &auwSystemGamma[0]);
    //ASSERT(bOK);
    ReleaseDC( NULL, hdc);
  }
#endif

  // free stocks
  delete _pEntityClassStock;  _pEntityClassStock = NULL;
  delete _pModelStock;        _pModelStock       = NULL; 
  delete _pSoundStock;        _pSoundStock       = NULL; 
  delete _pTextureStock;      _pTextureStock     = NULL; 
  delete _pAnimStock;         _pAnimStock        = NULL; 
  delete _pMeshStock;         _pMeshStock        = NULL; 
  delete _pSkeletonStock;     _pSkeletonStock    = NULL; 
  delete _pAnimSetStock;      _pAnimSetStock     = NULL; 
  delete _pShaderStock;       _pShaderStock      = NULL; 

  // free all memory used by the crc cache
  CRCT_Clear();

  // shutdown
  delete _pInput;    _pInput   = NULL;  
  delete _pSound;    _pSound   = NULL;  
  delete _pGfx;      _pGfx     = NULL;    
  if (_pNetwork != NULL) { delete _pNetwork; _pNetwork = NULL; }
  delete _pTimer;    _pTimer   = NULL;  
  delete _pShell;    _pShell   = NULL;  
  delete _pConsole;  _pConsole = NULL;
  extern void EndStreams(void);
  EndStreams();

  // shutdown profilers
  _sfStats.Clear();
  _pfGfxProfile           .pf_apcCounters.Clear();
  _pfGfxProfile           .pf_aptTimers  .Clear();
  _pfModelProfile         .pf_apcCounters.Clear();
  _pfModelProfile         .pf_aptTimers  .Clear();
  _pfSoundProfile         .pf_apcCounters.Clear();
  _pfSoundProfile         .pf_aptTimers  .Clear();
  _pfNetworkProfile       .pf_apcCounters.Clear();
  _pfNetworkProfile       .pf_aptTimers  .Clear();
  _pfRenderProfile        .pf_apcCounters.Clear();
  _pfRenderProfile        .pf_aptTimers  .Clear();
  _pfWorldEditingProfile  .pf_apcCounters.Clear();
  _pfWorldEditingProfile  .pf_aptTimers  .Clear();
  _pfPhysicsProfile       .pf_apcCounters.Clear();
  _pfPhysicsProfile       .pf_aptTimers  .Clear();

  // remove default fonts if needed
  if( _pfdDisplayFont != NULL) { delete _pfdDisplayFont;  _pfdDisplayFont=NULL; }
  if( _pfdConsoleFont != NULL) { delete _pfdConsoleFont;  _pfdConsoleFont=NULL; } 

  // deinit IFeel
  IFeel_DeleteDevice();

  // [Cecil] SDL: Shutdown
  SE_EndSDL();
}


// prepare and load some default fonts
ENGINE_API void SE_LoadDefaultFonts(void)
{
  _pfdDisplayFont = new CFontData;
  _pfdConsoleFont = new CFontData;

  // try to load default fonts
  try {
    _pfdDisplayFont->Load_t( CTFILENAME( "Fonts\\Display3-narrow.fnt"));
    _pfdConsoleFont->Load_t( CTFILENAME( "Fonts\\Console1.fnt"));
  }
  catch (char *strError) {
    FatalError( TRANS("Error loading font: %s."), strError);
  }
  // change fonts' default spacing a bit
  _pfdDisplayFont->SetCharSpacing( 0);
  _pfdDisplayFont->SetLineSpacing(+1);
  _pfdConsoleFont->SetCharSpacing(-1);
  _pfdConsoleFont->SetLineSpacing(+1);
  _pfdConsoleFont->SetFixedWidth();
}


// updates main windows' handles for windowed mode and fullscreen
ENGINE_API void SE_UpdateWindowHandle(OS::Window hwndMain)
{
  ASSERT( hwndMain!=NULL);
  _hwndCurrent = hwndMain;
  _bFullScreen = _pGfx!=NULL && (_pGfx->gl_ulFlags&GLF_FULLSCREEN);
}

// [Cecil] Don't care about pretouching
#if !SE1_OLD_COMPILER

BOOL _bNeedPretouch = FALSE;

ENGINE_API extern void SE_PretouchIfNeeded(void)
{
  extern INDEX gam_bPretouch;
  gam_bPretouch = FALSE;
  _bNeedPretouch = FALSE;
};

#else

static BOOL TouchBlock(UBYTE *pubMemoryBlock, INDEX ctBlockSize)
{
  // cannot pretouch block that are smaller than 64KB :(
  ctBlockSize -= 16*0x1000;
  if( ctBlockSize<4) return FALSE; 

  __try {
    // 4 times should be just enough
    for( INDEX i=0; i<4; i++) {
      // must do it in asm - don't know what VC will try to optimize
      __asm {
        // The 16-page skip is to keep Win 95 from thinking we're trying to page ourselves in
        // (we are doing that, of course, but there's no reason we shouldn't) - THANX JOHN! :)
        mov   esi,dword ptr [pubMemoryBlock]
        mov   ecx,dword ptr [ctBlockSize]
        shr   ecx,2
touchLoop:
        mov   eax,dword ptr [esi]
        mov   ebx,dword ptr [esi+16*0x1000]
        add   eax,ebx     // BLA, BLA, TROOCH, TRUCH
        add   esi,4
        dec   ecx
        jnz   touchLoop
      }
    }
  }
  __except(EXCEPTION_EXECUTE_HANDLER) { 
    return FALSE;
  }
  return TRUE;
}


// pretouch all memory commited by process
BOOL _bNeedPretouch = FALSE;
ENGINE_API extern void SE_PretouchIfNeeded(void)
{
  // only if pretouching is needed?
  extern INDEX gam_bPretouch;
  if( !_bNeedPretouch || !gam_bPretouch) return;
  _bNeedPretouch = FALSE;

  // set progress bar
  SetProgressDescription( TRANS("pretouching"));
  CallProgressHook_t(0.0f);

  // need to do this two times - 1st for numerations, and 2nd for real (progress bar and that shit)
  BOOL bPretouched = TRUE;
  INDEX ctFails, ctBytes, ctBlocks;
  INDEX ctPassBytes, ctTotalBlocks;
  for( INDEX iPass=1; iPass<=2; iPass++)
  { 
    // flush variables
    ctFails=0; ctBytes=0; ctBlocks=0; ctTotalBlocks=0;
    void *pvNextBlock = NULL;
    MEMORY_BASIC_INFORMATION mbi;
    // lets walk thru memory blocks
    while( VirtualQuery( pvNextBlock, &mbi, sizeof(mbi)))
    { 
      // don't mess with kernel's memory and zero-sized blocks    
      if( ((ULONG)pvNextBlock)>0x7FFF0000UL || mbi.RegionSize<1) break;

      // if this region of memory belongs to our process
      BOOL bCanAccess = (mbi.Protect==PAGE_READWRITE); // || (mbi.Protect==PAGE_EXECUTE_READWRITE);
      if( mbi.State==MEM_COMMIT && bCanAccess && mbi.Type==MEM_PRIVATE) // && !IsBadReadPtr( mbi.BaseAddress, 1)
      { 
        // increase counters
        ctBlocks++;
        ctBytes += mbi.RegionSize;
        // in first pass we only count
        if( iPass==1) goto nextRegion;
        // update progress bar
        CallProgressHook_t( (FLOAT)ctBytes/ctPassBytes);
        // pretouch
        ASSERT( mbi.RegionSize>0);
        BOOL bOK = TouchBlock((UBYTE *)mbi.BaseAddress, mbi.RegionSize);
        if( !bOK) { 
          // whoops!
          ctFails++;
        }
        // for easier debugging (didn't help much, though)
        //_pTimer->Suspend(5);
      }
nextRegion:
      // advance to next region
      pvNextBlock = ((UBYTE*)mbi.BaseAddress) + mbi.RegionSize;
      ctTotalBlocks++;
    }
    // done with one pass
    ctPassBytes = ctBytes;
    if( (ctPassBytes/1024/1024)>sys_iRAMPhys) {
      // not enough RAM, sorry :(
      bPretouched = FALSE;
      break;
    }
  }

  // report
  if( bPretouched) {
    // success
    CPrintF( TRANS("Pretouched %d KB of memory in %d blocks.\n"), ctBytes/1024, ctBlocks); //, ctTotalBlocks);
  } else {
    // fail
    CPrintF( TRANS("Cannot pretouch due to lack of physical memory (%d KB of overflow).\n"), ctPassBytes/1024-sys_iRAMPhys*1024);
  }
  // some blocks failed?
  if( ctFails>1) CPrintF( TRANS("(%d blocks were skipped)\n"), ctFails);
  //_pShell->Execute("StockDump();");
}
#endif
