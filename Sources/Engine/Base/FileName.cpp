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

#include "stdh.h"

#include <Engine/Base/FileName.h>

#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Stream.h>
#include <Engine/Templates/NameTable_CTFileName.h>
#include <Engine/Templates/DynamicStackArray.cpp>
#include <Engine/Templates/StaticStackArray.cpp>

/*
 * Get directory part of a filename.
 */
CTString CTString::FileDir() const
{
  ASSERT(IsValid());

  // make a temporary copy of string
  CTString strPath(*this);
  // find last backlash in it
  char *pPathBackSlash = strrchr(strPath.Data(), '\\');
  // if there is no backslash
  if( pPathBackSlash == NULL) {
    // return emptystring as directory
    return "";
  }
  // set end of string after where the backslash was
  pPathBackSlash[1] = '\0';
  // return a copy of temporary string
  return strPath;
}

/*
 * Get name part of a filename.
 */
CTString CTString::FileName() const
{
  ASSERT(IsValid());

  // make a temporary copy of string
  CTString strPath(*this);
  // find last dot in it
  char *pDot = strrchr(strPath.Data(), '.');
  // if there is a dot
  if( pDot != NULL) {
    // set end of string there
    pDot[0] = '\0';
  }

  // find last backlash in what's left
  const char *pBackSlash = strrchr(strPath.ConstData(), '\\');
  // if there is no backslash
  if( pBackSlash == NULL) {
    // return it all as filename
    return strPath;
  }
  // return a copy of temporary string, starting after the backslash
  return CTString(pBackSlash + 1);
}

/*
 * Get extension part of a filename.
 */
CTString CTString::FileExt() const
{
  ASSERT(IsValid());

  // find last dot in the string
  const char *pExtension = strrchr(ConstData(), '.');
  // if there is no dot
  if( pExtension == NULL) {
    // return no extension
    return "";
  }
  // return a copy of the extension part, together with the dot
  return pExtension;
}

CTString CTString::NoExt() const
{
  return FileDir()+FileName();
}

static INDEX GetSlashPosition(const char *pszString)
{
  for (INDEX iPos = 0; '\0' != *pszString; ++iPos, ++pszString) {
    if (('\\' == *pszString) || ('/' == *pszString)) {
      return iPos;
    }
  }
  return -1;
}

/*
 * Set path to the absolute path, taking \.. and /.. into account.
 */
void CTString::SetAbsolutePath(void)
{
  // Collect path parts
  CTString strRemaining(*this);
  CStaticStackArray<CTString> astrParts;
  INDEX iSlashPos = GetSlashPosition(strRemaining.ConstData());
  if (0 > iSlashPos) {
    return; // Invalid path
  }
  for (;;) {
    CTString &strBeforeSlash = astrParts.Push();
    CTString strAfterSlash;
    strRemaining.Split(iSlashPos, strBeforeSlash, strAfterSlash);
    strAfterSlash.TrimLeft(strAfterSlash.Length() - 1);
    strRemaining = strAfterSlash;
    iSlashPos = GetSlashPosition(strRemaining.ConstData());
    if (0 > iSlashPos) {
      astrParts.Push() = strRemaining;
      break;
    }
  }
  // Remove certain path parts
  INDEX iPart;
  for (iPart = 0; iPart < astrParts.Count(); ++iPart) {
    if (CTString("..") != astrParts[iPart]) {
      continue;
    }
    if (0 == iPart) {
      return; // Invalid path
    }
    // Remove ordered
    CStaticStackArray<CTString> astrShrinked;
    astrShrinked.Push(astrParts.Count() - 2);
    astrShrinked.PopAll();
    for (INDEX iCopiedPart = 0; iCopiedPart < astrParts.Count(); ++iCopiedPart) {
      if ((iCopiedPart != iPart - 1) && (iCopiedPart != iPart)) {
        astrShrinked.Push() = astrParts[iCopiedPart];
      }
    }
    astrParts.MoveArray(astrShrinked);
    iPart -= 2;
  }
  // Set new content
  strRemaining.Clear();
  for (iPart = 0; iPart < astrParts.Count(); ++iPart) {
    strRemaining += astrParts[iPart];
    if (iPart < astrParts.Count() - 1) {
      strRemaining += "\\";
    }
  }
  (*this) = strRemaining;
}

/*
 * Remove application path from a file name and returns TRUE if it's a relative path.
 */
BOOL CTString::RemoveApplicationPath_t(void) // throws char *
{
  CTString fnmApp = _fnmApplicationPath;
  fnmApp.SetAbsolutePath();
  // remove the path string from beginning of the string
  BOOL bIsRelative = RemovePrefix(fnmApp);
  if (_fnmMod!="") {
    RemovePrefix(_fnmApplicationPath+_fnmMod);
  }
  return bIsRelative;
}

// [Cecil] Convert from a relative path to an absolute path and add missing backslashes
void CTString::SetFullDirectory(void) {
  CTString &str = *this;
  INDEX iLength = str.Length();

  // Add missing backslash at the end
  if (iLength > 0 && str[iLength - 1] != '\\') {
    str += CTString("\\");
  }

  // If shorter than 2 characters or doesn't start with a drive directory
  if (iLength < 2 || str[1] != ':') {
    // Convert relative path into absolute path
    str = _fnmApplicationPath + str;
  }

  // Convert the rest of the path into absolute path
  str.SetAbsolutePath();
};

// [Cecil] Read string as a filename from a stream
void CTStream::ReadFileName(CTString &fnmFileName)
{
  // if dictionary is enabled
  if (strm_dmDictionaryMode == CTStream::DM_ENABLED) {
    // read the index in dictionary
    INDEX iFileName;
    *this >> iFileName;
    // get that file from the dictionary
    fnmFileName = strm_afnmDictionary[iFileName];

  // if dictionary is processing or not active
  } else {
    char strTag[] = "_FNM"; strTag[0] = 'D';  // must create tag at run-time!
    // skip dependency catcher header
    ExpectID_t(strTag);

    // read the string
    *this >> fnmFileName;
  }
};

// [Cecil] Write string as a filename into a stream
void CTStream::WriteFileName(const CTString &fnmFileName)
{
  // if dictionary is enabled
  if (strm_dmDictionaryMode == CTStream::DM_ENABLED) {
    // try to find the filename in dictionary
    CTString *pfnmExisting = strm_ntDictionary.Find(fnmFileName);
    // if not existing
    if (pfnmExisting==NULL) {
      // add it
      pfnmExisting = &strm_afnmDictionary.Push();
      *pfnmExisting = fnmFileName;
      strm_ntDictionary.Add(pfnmExisting);
    }
    // write its index
    *this << strm_afnmDictionary.Index(pfnmExisting);

  // if dictionary is processing or not active
  } else {
    char strTag[] = "_FNM"; strTag[0] = 'D';  // must create tag at run-time!
    // write dependency catcher header
    WriteID_t(strTag);

    // write the string
    *this << fnmFileName;
  }
};
