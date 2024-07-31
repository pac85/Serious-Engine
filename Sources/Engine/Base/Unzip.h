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

#ifndef SE_INCL_UNZIP_H
#define SE_INCL_UNZIP_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

// [Cecil] A file entry in a ZIP archive (moved from Unzip.cpp)
class CZipEntry {
  public:
    const CTFileName *ze_pfnmArchive; // Path of the archive
    CTFileName ze_fnm;           // File name with path inside archive
    SLONG ze_slCompressedSize;   // Size of file in the archive
    SLONG ze_slUncompressedSize; // Size when uncompressed
    SLONG ze_slDataOffset;       // Position of compressed data inside archive
    ULONG ze_ulCRC;              // Checksum of the file
    BOOL ze_bStored;             // Set if file is not compressed, but stored
    BOOL ze_bMod;                // Set if from a mod's archive

  public:
    void Clear(void) {
      ze_pfnmArchive = NULL;
      ze_fnm.Clear();
    };
};

// add one zip archive to current active set
void UNZIPAddArchive(const CTFileName &fnm);
// read directories of all currently added archives, in reverse alphabetical order
void UNZIPReadDirectoriesReverse_t(void);
// check if a zip file entry exists
BOOL UNZIPFileExists(const CTFileName &fnm);
// open a zip file entry for reading
INDEX UNZIPOpen_t(const CTFileName &fnm);
// get uncompressed size of a file
SLONG UNZIPGetSize(INDEX iHandle);
// get CRC of a file
ULONG UNZIPGetCRC(INDEX iHandle);
// read a block from zip file
void UNZIPReadBlock_t(INDEX iHandle, UBYTE *pub, SLONG slStart, SLONG slLen);
// close a zip file entry
void UNZIPClose(INDEX iHandle);
// get info on a zip file entry
void UNZIPGetFileInfo(INDEX iHandle, CTFileName &fnmZip,
  SLONG &slOffset, SLONG &slSizeCompressed, SLONG &slSizeUncompressed, 
  BOOL &bCompressed);
// enumeration for all files in all zips
INDEX UNZIPGetFileCount(void);
const CTFileName &UNZIPGetFileAtIndex(INDEX i);

// [Cecil] Get ZIP file entry at a specific position
const CZipEntry &UNZIPGetEntry(INDEX i);

// get index of a file (-1 for no file)
INDEX UNZIPGetFileIndex(const CTFileName &fnm);
// check if a file is from a mod's zip
BOOL UNZIPIsFileAtIndexMod(INDEX i);


#endif  /* include-once check. */

