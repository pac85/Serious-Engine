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

#ifndef SE_INCL_CTSTRING_H
#define SE_INCL_CTSTRING_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#include <Engine/Base/Types.h>

/*
 * Main string class
 */
class ENGINE_API CTString {
public:
  char *str_String;         // pointer to memory holding the character string
public:
  /* Default constructor. */
  inline CTString(void);
  /* Copy constructor. */
  inline CTString(const CTString &strOriginal);
  /* Constructor from character string. */
  inline CTString(const char *pString);
  /* Constructor with formatting. */
  inline CTString(INDEX iDummy, const char *strFormat, ...);
  /* Destructor. */
  inline ~CTString();
  /* Clear the object. */
  inline void Clear(void);

  /* Conversion into character string. */
  inline operator const char*() const;

  // [Cecil] Retrieve string data
  inline char *Data(void) {
    return str_String;
  };

  // [Cecil] Retrieve constant string data
  inline const char *ConstData(void) const {
    return str_String;
  };

  /* Assignment. */
  inline CTString &operator=(const char *strCharString);
  inline CTString &operator=(const CTString &strOther);

  /* Check if string data is valid. */
  BOOL IsValid(void) const;

  // return length of the string
  inline INDEX Length(void) const { return (INDEX)strlen(str_String); };
  INDEX LengthNaked(void) const;

  // strip decorations from the string
  CTString Undecorated(void) const;

  // [Cecil] Find substring in a string
  inline const char *Substr(const char *strSub, size_t iFrom = 0) const {
    return strstr(str_String + iFrom, strSub);
  };

  // [Cecil] Find index of a substring in a string (-1 if not found)
  inline size_t FindSubstr(const char *strSub, size_t iFrom = 0) const {
    const char *str = Substr(strSub, iFrom);

    if (str == NULL) return -1;
    
    return str - str_String;
  };

  /* Replace a substring in a string. */
  BOOL ReplaceSubstr(const CTString &strSub, const CTString &strNewSub);

  // [Cecil] Find character in a string
  inline char *FindChar(char ch, size_t iFrom = 0) {
    return strchr(str_String + iFrom, ch);
  };

  // [Cecil] Find character in a constant string
  inline const char *FindChar(char ch, size_t iFrom = 0) const {
    return strchr(str_String + iFrom, ch);
  };

  // [Cecil] Find character position in a string (-1 if not found)
  inline size_t FindCharIndex(char ch, size_t iFrom = 0) const {
    const char *pch = FindChar(ch, iFrom);

    if (pch == NULL) return -1;

    return pch - str_String;
  };

  // [Cecil] Replace specific character in the entire string
  void ReplaceChar(char chOld, char chNew);

  /* Check if has given prefix */
  BOOL HasPrefix( const CTString &strPrefix) const;
  /* Remove given prefix string from this string */
  BOOL RemovePrefix( const CTString &strPrefix);
  /* Trim the string to contain at most given number of characters. */
  INDEX TrimLeft(  INDEX ctCharacters);
  INDEX TrimRight( INDEX ctCharacters);
  /* Trim the string from spaces. */
  INDEX TrimSpacesLeft(void);
  INDEX TrimSpacesRight(void);
  /* Calcuate hashing value for the string. */
  ULONG GetHash(void) const;
  // retain only first line of the string
  void OnlyFirstLine(void);

  // [Cecil] Convert all characters to lowercase
  CTString ToLower(void) const;

  // [Cecil] Convert all characters to uppercase
  CTString ToUpper(void) const;

  /* Equality comparison. */
  BOOL operator==(const CTString &strOther) const;
  BOOL operator==(const char *strOther) const;
  ENGINE_API friend BOOL operator==(const char *strThis, const CTString &strOther);
  /* Inequality comparison. */
  BOOL operator!=(const CTString &strOther) const;
  BOOL operator!=(const char *strOther) const;
  ENGINE_API friend BOOL operator!=(const char *strThis, const CTString &strOther);
  // wild card comparison (other string may contain wildcards)
  BOOL Matches(const CTString &strOther) const;
  BOOL Matches(const char *strOther) const;

  /* String concatenation. */
  CTString operator+(const CTString &strSecond) const;
  CTString &operator+=(const CTString &strSecond);
  ENGINE_API friend CTString operator+(const char *strFirst, const CTString &strSecond);

  // split string in two strings at specified position (char AT splitting position goes to str2)
  void Split( INDEX iPos, CTString &str1, CTString &str2);
  void InsertChar( INDEX iPos, char cChr); // insert char at position
  void DeleteChar( INDEX iPos); // delete char at position
  
  /* Throw exception */
  void Throw_t(void);

  /* Read from stream. */
  ENGINE_API friend CTStream &operator>>(CTStream &strmStream, CTString &strString);
  void ReadFromText_t(CTStream &strmStream, const CTString &strKeyword); // throw char *
  /* Write to stream. */
  ENGINE_API friend CTStream &operator<<(CTStream &strmStream, const CTString &strString);

  /* Load an entire text file into a string. */
  void Load_t(const class CTFileName &fnmFile);  // throw char *
  void LoadKeepCRLF_t(const class CTFileName &fnmFile);  // throw char *
  void ReadUntilEOF_t(CTStream &strmStream);  // throw char *
  /* Save an entire string into a text file. */
  void Save_t(const class CTFileName &fnmFile);  // throw char *
  void SaveKeepCRLF_t(const class CTFileName &fnmFile);  // throw char *

  // Print formatted to a string
  INDEX PrintF(const char *strFormat, ...);
  INDEX VPrintF(const char *strFormat, va_list arg);
  // Scan formatted from a string
  INDEX ScanF(const char *strFormat, ...) const;

  // variable management functions
  void LoadVar(const CTFileName &fnmFile);
  void SaveVar(const CTFileName &fnmFile);
};


// general variable functions
ENGINE_API void LoadStringVar( const CTFileName &fnmVar, CTString &strVar);
ENGINE_API void SaveStringVar( const CTFileName &fnmVar, CTString &strVar);
ENGINE_API void LoadIntVar(    const CTFileName &fnmVar, INDEX &iVar);
ENGINE_API void SaveIntVar(    const CTFileName &fnmVar, INDEX &iVar);

ENGINE_API CTString RemoveSpecialCodes( const CTString &str);


#include <Engine/Base/CTString.inl>


#endif  /* include-once check. */

