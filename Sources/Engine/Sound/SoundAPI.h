/* Copyright (c) 2024 Dreamy Cecil
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

#ifndef SE_INCL_SOUND_INTERFACE_H
#define SE_INCL_SOUND_INTERFACE_H

// [Cecil] Windows-specific
#if SE1_WIN
  #if !SE1_OLD_COMPILER
    #include <initguid.h>
  #endif

  // [Cecil] Needed here for EAX_ENVIRONMENT_* enum values
  #include <Engine/Sound/DSound.h>
  #include <Engine/Sound/EAX.h>
#endif

class ENGINE_API CAbstractSoundAPI {
  public:
    enum ESoundAPI {
      E_SND_INVALID = -1,

      E_SND_WAVEOUT = 0,
      E_SND_DSOUND,
      E_SND_EAX,

      E_SND_MAX,
    };

  public:
    SLONG *m_pslMixerBuffer;     // buffer for mixing sounds (32-bit!)
    SLONG  m_slMixerBufferSize;  // mixer buffer size

    SWORD *m_pswDecodeBuffer;    // buffer for decoding encoded sounds (ogg, mpeg...)
    SLONG  m_slDecodeBufferSize; // decoder buffer size

  public:
    // Constructor
    CAbstractSoundAPI();

    // Destructor
    virtual ~CAbstractSoundAPI();

    // Allocate new buffer memory
    void AllocBuffers(void);

    // Free buffer memory
    void FreeBuffers(void);

    // Get API name from type
    static const CTString &GetApiName(ESoundAPI eAPI);

    // Create API from type
    static CAbstractSoundAPI *CreateAPI(ESoundAPI eAPI);

    // Sound interfaces should always return a distinct type
    virtual ESoundAPI GetType(void) {
      ASSERTALWAYS("Sound interface shouldn't return E_SND_INVALID as its type!");
      return E_SND_INVALID;
    };

  public:
    virtual BOOL StartUp(BOOL bReport = TRUE) = 0;

    // Free memory by default
    virtual void ShutDown(void) {
      FreeBuffers();
    };

    // Copy mixer buffer to sound buffers
    virtual void CopyMixerBuffer(SLONG slMixedSize) = 0;

    // Find room in sound buffer to copy in next crop of samples
    virtual SLONG PrepareSoundBuffer(void) = 0;

    // Assume that it will mute itself eventually by default
    virtual void Mute(void) {};

    // No EAX support by default
    virtual void UpdateEAX(void) {};
};

#endif // include-once check
