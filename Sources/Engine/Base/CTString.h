/* Copyright (c) 2002-2012 Croteam Ltd. 
   Copyright (c) 2023-2024 Dreamy Cecil
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

  static const size_t npos; // [Cecil] Invalid character index

public:
  /* Default constructor. */
  inline CTString(void);
  /* Copy constructor. */
  inline CTString(const CTString &strOriginal);
  // [Cecil] Constructor from character string with optional offset and amount of characters
  inline CTString(const char *strCharString, size_t iFrom = 0, size_t ct = npos);
  /* Constructor with formatting. */
  inline CTString(INDEX iDummy, const char *strFormat, ...);
  /* Destructor. */
  inline ~CTString();
  /* Clear the object. */
  inline void Clear(void);

  /* Conversion into character string. */
  inline operator const char*() const;

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

  /* Replace a substring in a string. */
  BOOL ReplaceSubstr(const CTString &strSub, const CTString &strNewSub);

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
  void Split( INDEX iPos, CTString &str1, CTString &str2) const;
  void InsertChar( INDEX iPos, char cChr); // insert char at position
  void DeleteChar( INDEX iPos); // delete char at position
  
  /* Throw exception */
  void Throw_t(void);

  /* Read from stream. */
  ENGINE_API friend CTStream &operator>>(CTStream &strmStream, CTString &strString);
  // [Cecil] Read filename flag
  void ReadFromText_t(CTStream &strmStream, const CTString &strKeyword, BOOL bFileName = FALSE);
  /* Write to stream. */
  ENGINE_API friend CTStream &operator<<(CTStream &strmStream, const CTString &strString);

  /* Load an entire text file into a string. */
  void Load_t(const CTString &fnmFile);  // throw char *
  void LoadKeepCRLF_t(const CTString &fnmFile);  // throw char *
  void ReadUntilEOF_t(CTStream &strmStream);  // throw char *
  /* Save an entire string into a text file. */
  void Save_t(const CTString &fnmFile);  // throw char *
  void SaveKeepCRLF_t(const CTString &fnmFile);  // throw char *

  // Print formatted to a string
  INDEX PrintF(const char *strFormat, ...);
  INDEX VPrintF(const char *strFormat, va_list arg);
  // Scan formatted from a string
  INDEX ScanF(const char *strFormat, ...) const;

  // variable management functions
  void LoadVar(const CTString &fnmFile);
  void SaveVar(const CTString &fnmFile);

// [Cecil] Extra methods
public:

  // Retrieve string data
  inline char *Data(void) {
    return str_String;
  };

  // Retrieve constant string data
  inline const char *ConstData(void) const {
    return str_String;
  };

  // Assign a new string consisting of a specific character
  inline void Fill(size_t ct, char ch);

  // Convert all characters to lowercase
  CTString ToLower(void) const;

  // Convert all characters to uppercase
  CTString ToUpper(void) const;

  // Erase a portion of characters from the middle of the string
  CTString &Erase(size_t iFrom, size_t ct = npos);

  // Extract substring from the string
  __forceinline CTString Substr(size_t iFrom, size_t ct = npos) const {
    return CTString(str_String, iFrom, ct);
  };

// [Cecil] New methods for searching
public:

  // Find substring within the string
  const char *GetSubstr(const char *strSub, size_t iFrom = 0) const;

  // Find character within the string
  char *GetChar(char ch, size_t iFrom = 0) const;

  // Find substring position in a string
  size_t Find(const char *strSub, size_t iFrom = 0) const;

  // Find character position in a string
  size_t Find(char ch, size_t iFrom = 0) const;

  // Deprecated method left for compatibility
  INDEX FindSubstr(const CTString &strSub) {
    return (INDEX)Find(strSub.str_String);
  };

  // Find last substring position in a string
  size_t RFind(const char *strSub, size_t iFrom = npos) const;

  // Find last character position in a string
  size_t RFind(char ch, size_t iFrom = npos) const;

  // Find first occurrence of any of the characters
  size_t FindFirstOf(const char *str, size_t iFrom = 0) const;

  // Find first absence of any of the characters
  size_t FindFirstNotOf(const char *str, size_t iFrom = 0) const;

  // Find last occurrence of any of the characters
  size_t FindLastOf(const char *str, size_t iFrom = npos) const;

  // Find last absence of any of the characters
  size_t FindLastNotOf(const char *str, size_t iFrom = npos) const;

// [Cecil] Migrated methods from CTFileName
public:

  // Get directory part of a filename
  CTString FileDir(void) const;
  // Get name part of a filename
  CTString FileName(void) const;
  // Get extension part of a filename
  CTString FileExt(void) const;
  // Get path and file name without extension
  CTString NoExt(void) const;
  // Set path to the absolute path, taking \.. and /.. into account
  void SetAbsolutePath(void);
  // Remove application path from a file name
  BOOL RemoveApplicationPath_t(void);

  // [Cecil] Convert from a relative path to an absolute path and add missing backslashes
  void SetFullDirectory(void);

  // Filename is its own name (used for storing in nametable)
  inline const CTString &GetName(void) const {
    return *this;
  };
};

// [Cecil] Alias for compatibility
typedef CTString CTFileName;

// [Cecil] Migrated macros for defining a literal filename in code (EFNM = EXE filename)
#define CTFILENAME(string) CTString("EFNM" string, 4)
#define DECLARE_CTFILENAME(name, string) CTString name("EFNM" string, 4)

// general variable functions
ENGINE_API void LoadStringVar( const CTFileName &fnmVar, CTString &strVar);
ENGINE_API void SaveStringVar( const CTFileName &fnmVar, CTString &strVar);
ENGINE_API void LoadIntVar(    const CTFileName &fnmVar, INDEX &iVar);
ENGINE_API void SaveIntVar(    const CTFileName &fnmVar, INDEX &iVar);

ENGINE_API CTString RemoveSpecialCodes( const CTString &str);


#include <Engine/Base/CTString.inl>

// [Cecil] Hashing function for compatibility with STL
namespace std {
  template<class Key> struct hash;

  template<> struct hash<CTString> {
    inline size_t operator()(const CTString &str) const noexcept {
      return str.GetHash();
    };
  };
};

#endif  /* include-once check. */

