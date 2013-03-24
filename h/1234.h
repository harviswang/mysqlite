/******************************************************************************
** This file is an amalgamation of many separate C source files from SQLite
** version 3.7.15.2.  By combining all the individual C code files into this 
** single large file, the entire code can be compiled as a single translation
** unit.  This allows many compilers to do optimizations that would not be
** possible if the files were compiled separately.  Performance improvements
** of 5% or more are commonly seen when SQLite is compiled as a single
** translation unit.
**
** This file is all you need to compile SQLite.  To use SQLite in other
** programs, you need this file and the "sqlite3.h" header file that defines
** the programming interface to the SQLite library.  (If you do not have 
** the "sqlite3.h" header file at hand, you will find a copy embedded within
** the text of this file.  Search for "Begin file sqlite3.h" to find the start
** of the embedded sqlite3.h header file.) Additional code files may be needed
** if you want a wrapper to interface SQLite with your choice of programming
** language. The code for the "sqlite3" command-line shell is also in a
** separate file. This file contains only code for the core SQLite library.
*/
#define SQLITE_CORE 1
#define SQLITE_AMALGAMATION 1
#ifndef SQLITE_PRIVATE
# define SQLITE_PRIVATE
#endif
#ifndef SQLITE_API
# define SQLITE_API
#endif

#include "sqliteInt.h"
#include "sqliteLimit.h"
#include "vdbe.h"
#include "vdbeInt.h"



#include "fts3.h"
#include "sqlite3.h"
#include "btree.h"
#include "btreeInt.h"
#include "fts3.h"
#include "fts3_hash.h"
#include "fts3_tokenizer.h"
#include "fts3Int.h"
#include "hash.h"
#include "hwtime.h"
#include "keywordhash.h"
#include "mutex.h"
#include "opcodes.h"
#include "os.h"
#include "os_common.h"
#include "pager.h"
#include "parse.h"
#include "pcache.h"
#include "rtree.h"
#include "sqlite3.h"
#include "sqlite3ext.h"
#include "sqliteicu.h"
#include "wal.h"

extern const unsigned char sqlite3CtypeMap[256];
extern SQLITE_WSD struct Sqlite3Config sqlite3Config;
extern SQLITE_WSD FuncDefHash sqlite3GlobalFunctions;
extern int sqlite3PendingByte;
extern const unsigned char sqlite3UpperToLower[256];
extern const Token sqlite3IntTokens[2];
extern const unsigned char sqlite3OpcodeProperty[];




