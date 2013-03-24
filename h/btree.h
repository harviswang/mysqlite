/************** Begin file btree.h *******************************************/
/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This header file defines the interface that the sqlite B-Tree file
** subsystem.  See comments in the source code for a detailed description
** of what each interface routine does.
*/
#ifndef _BTREE_H_
#define _BTREE_H_

/* TODO: This definition is just included so other modules compile. It
** needs to be revisited.
*/
#define SQLITE_N_BTREE_META 10

/*
** If defined as non-zero, auto-vacuum is enabled by default. Otherwise
** it must be turned on for each database using "PRAGMA auto_vacuum = 1".
*/
#ifndef SQLITE_DEFAULT_AUTOVACUUM
  #define SQLITE_DEFAULT_AUTOVACUUM 0
#endif

#define BTREE_AUTOVACUUM_NONE 0        /* Do not do auto-vacuum */
#define BTREE_AUTOVACUUM_FULL 1        /* Do full auto-vacuum */
#define BTREE_AUTOVACUUM_INCR 2        /* Incremental vacuum */

/*
** Forward declarations of structure
*/
typedef struct Btree Btree;
typedef struct BtCursor BtCursor;
typedef struct BtShared BtShared;


SQLITE_PRIVATE int sqlite3BtreeOpen(
  sqlite3_vfs *pVfs,       /* VFS to use with this b-tree */
  const char *zFilename,   /* Name of database file to open */
  sqlite3 *db,             /* Associated database connection */
  Btree **ppBtree,         /* Return open Btree* here */
  int flags,               /* Flags */
  int vfsFlags             /* Flags passed through to VFS open */
);

/* The flags parameter to sqlite3BtreeOpen can be the bitwise or of the
** following values.
**
** NOTE:  These values must match the corresponding PAGER_ values in
** pager.h.
*/
#define BTREE_OMIT_JOURNAL  1  /* Do not create or use a rollback journal */
#define BTREE_MEMORY        2  /* This is an in-memory DB */
#define BTREE_SINGLE        4  /* The file contains at most 1 b-tree */
#define BTREE_UNORDERED     8  /* Use of a hash implementation is OK */

SQLITE_PRIVATE int sqlite3BtreeClose(Btree*);
SQLITE_PRIVATE int sqlite3BtreeSetCacheSize(Btree*,int);
SQLITE_PRIVATE int sqlite3BtreeSetSafetyLevel(Btree*,int,int,int);
SQLITE_PRIVATE int sqlite3BtreeSyncDisabled(Btree*);
SQLITE_PRIVATE int sqlite3BtreeSetPageSize(Btree *p, int nPagesize, int nReserve, int eFix);
SQLITE_PRIVATE int sqlite3BtreeGetPageSize(Btree*);
SQLITE_PRIVATE int sqlite3BtreeMaxPageCount(Btree*,int);
SQLITE_PRIVATE u32 sqlite3BtreeLastPage(Btree*);
SQLITE_PRIVATE int sqlite3BtreeSecureDelete(Btree*,int);
SQLITE_PRIVATE int sqlite3BtreeGetReserve(Btree*);
#if defined(SQLITE_HAS_CODEC) || defined(SQLITE_DEBUG)
SQLITE_PRIVATE int sqlite3BtreeGetReserveNoMutex(Btree *p);
#endif
SQLITE_PRIVATE int sqlite3BtreeSetAutoVacuum(Btree *, int);
SQLITE_PRIVATE int sqlite3BtreeGetAutoVacuum(Btree *);
SQLITE_PRIVATE int sqlite3BtreeBeginTrans(Btree*,int);
SQLITE_PRIVATE int sqlite3BtreeCommitPhaseOne(Btree*, const char *zMaster);
SQLITE_PRIVATE int sqlite3BtreeCommitPhaseTwo(Btree*, int);
SQLITE_PRIVATE int sqlite3BtreeCommit(Btree*);
SQLITE_PRIVATE int sqlite3BtreeRollback(Btree*,int);
SQLITE_PRIVATE int sqlite3BtreeBeginStmt(Btree*,int);
SQLITE_PRIVATE int sqlite3BtreeCreateTable(Btree*, int*, int flags);
SQLITE_PRIVATE int sqlite3BtreeIsInTrans(Btree*);
SQLITE_PRIVATE int sqlite3BtreeIsInReadTrans(Btree*);
SQLITE_PRIVATE int sqlite3BtreeIsInBackup(Btree*);
SQLITE_PRIVATE void *sqlite3BtreeSchema(Btree *, int, void(*)(void *));
SQLITE_PRIVATE int sqlite3BtreeSchemaLocked(Btree *pBtree);
SQLITE_PRIVATE int sqlite3BtreeLockTable(Btree *pBtree, int iTab, u8 isWriteLock);
SQLITE_PRIVATE int sqlite3BtreeSavepoint(Btree *, int, int);

SQLITE_PRIVATE const char *sqlite3BtreeGetFilename(Btree *);
SQLITE_PRIVATE const char *sqlite3BtreeGetJournalname(Btree *);
SQLITE_PRIVATE int sqlite3BtreeCopyFile(Btree *, Btree *);

SQLITE_PRIVATE int sqlite3BtreeIncrVacuum(Btree *);

/* The flags parameter to sqlite3BtreeCreateTable can be the bitwise OR
** of the flags shown below.
**
** Every SQLite table must have either BTREE_INTKEY or BTREE_BLOBKEY set.
** With BTREE_INTKEY, the table key is a 64-bit integer and arbitrary data
** is stored in the leaves.  (BTREE_INTKEY is used for SQL tables.)  With
** BTREE_BLOBKEY, the key is an arbitrary BLOB and no content is stored
** anywhere - the key is the content.  (BTREE_BLOBKEY is used for SQL
** indices.)
*/
#define BTREE_INTKEY     1    /* Table has only 64-bit signed integer keys */
#define BTREE_BLOBKEY    2    /* Table has keys only - no data */

SQLITE_PRIVATE int sqlite3BtreeDropTable(Btree*, int, int*);
SQLITE_PRIVATE int sqlite3BtreeClearTable(Btree*, int, int*);
SQLITE_PRIVATE void sqlite3BtreeTripAllCursors(Btree*, int);

SQLITE_PRIVATE void sqlite3BtreeGetMeta(Btree *pBtree, int idx, u32 *pValue);
SQLITE_PRIVATE int sqlite3BtreeUpdateMeta(Btree*, int idx, u32 value);

SQLITE_PRIVATE int sqlite3BtreeNewDb(Btree *p);

/*
** The second parameter to sqlite3BtreeGetMeta or sqlite3BtreeUpdateMeta
** should be one of the following values. The integer values are assigned 
** to constants so that the offset of the corresponding field in an
** SQLite database header may be found using the following formula:
**
**   offset = 36 + (idx * 4)
**
** For example, the free-page-count field is located at byte offset 36 of
** the database file header. The incr-vacuum-flag field is located at
** byte offset 64 (== 36+4*7).
*/
#define BTREE_FREE_PAGE_COUNT     0
#define BTREE_SCHEMA_VERSION      1
#define BTREE_FILE_FORMAT         2
#define BTREE_DEFAULT_CACHE_SIZE  3
#define BTREE_LARGEST_ROOT_PAGE   4
#define BTREE_TEXT_ENCODING       5
#define BTREE_USER_VERSION        6
#define BTREE_INCR_VACUUM         7

/*
** Values that may be OR'd together to form the second argument of an
** sqlite3BtreeCursorHints() call.
*/
#define BTREE_BULKLOAD 0x00000001

SQLITE_PRIVATE int sqlite3BtreeCursor(
  Btree*,                              /* BTree containing table to open */
  int iTable,                          /* Index of root page */
  int wrFlag,                          /* 1 for writing.  0 for read-only */
  struct KeyInfo*,                     /* First argument to compare function */
  BtCursor *pCursor                    /* Space to write cursor structure */
);
SQLITE_PRIVATE int sqlite3BtreeCursorSize(void);
SQLITE_PRIVATE void sqlite3BtreeCursorZero(BtCursor*);

SQLITE_PRIVATE int sqlite3BtreeCloseCursor(BtCursor*);
SQLITE_PRIVATE int sqlite3BtreeMovetoUnpacked(
  BtCursor*,
  UnpackedRecord *pUnKey,
  i64 intKey,
  int bias,
  int *pRes
);
SQLITE_PRIVATE int sqlite3BtreeCursorHasMoved(BtCursor*, int*);
SQLITE_PRIVATE int sqlite3BtreeDelete(BtCursor*);
SQLITE_PRIVATE int sqlite3BtreeInsert(BtCursor*, const void *pKey, i64 nKey,
                                  const void *pData, int nData,
                                  int nZero, int bias, int seekResult);
SQLITE_PRIVATE int sqlite3BtreeFirst(BtCursor*, int *pRes);
SQLITE_PRIVATE int sqlite3BtreeLast(BtCursor*, int *pRes);
SQLITE_PRIVATE int sqlite3BtreeNext(BtCursor*, int *pRes);
SQLITE_PRIVATE int sqlite3BtreeEof(BtCursor*);
SQLITE_PRIVATE int sqlite3BtreePrevious(BtCursor*, int *pRes);
SQLITE_PRIVATE int sqlite3BtreeKeySize(BtCursor*, i64 *pSize);
SQLITE_PRIVATE int sqlite3BtreeKey(BtCursor*, u32 offset, u32 amt, void*);
SQLITE_PRIVATE const void *sqlite3BtreeKeyFetch(BtCursor*, int *pAmt);
SQLITE_PRIVATE const void *sqlite3BtreeDataFetch(BtCursor*, int *pAmt);
SQLITE_PRIVATE int sqlite3BtreeDataSize(BtCursor*, u32 *pSize);
SQLITE_PRIVATE int sqlite3BtreeData(BtCursor*, u32 offset, u32 amt, void*);
SQLITE_PRIVATE void sqlite3BtreeSetCachedRowid(BtCursor*, sqlite3_int64);
SQLITE_PRIVATE sqlite3_int64 sqlite3BtreeGetCachedRowid(BtCursor*);

SQLITE_PRIVATE char *sqlite3BtreeIntegrityCheck(Btree*, int *aRoot, int nRoot, int, int*);
SQLITE_PRIVATE struct Pager *sqlite3BtreePager(Btree*);

SQLITE_PRIVATE int sqlite3BtreePutData(BtCursor*, u32 offset, u32 amt, void*);
SQLITE_PRIVATE void sqlite3BtreeCacheOverflow(BtCursor *);
SQLITE_PRIVATE void sqlite3BtreeClearCursor(BtCursor *);
SQLITE_PRIVATE int sqlite3BtreeSetVersion(Btree *pBt, int iVersion);
SQLITE_PRIVATE void sqlite3BtreeCursorHints(BtCursor *, unsigned int mask);

#ifndef NDEBUG
SQLITE_PRIVATE int sqlite3BtreeCursorIsValid(BtCursor*);
#endif

#ifndef SQLITE_OMIT_BTREECOUNT
SQLITE_PRIVATE int sqlite3BtreeCount(BtCursor *, i64 *);
#endif

#ifdef SQLITE_TEST
SQLITE_PRIVATE int sqlite3BtreeCursorInfo(BtCursor*, int*, int);
SQLITE_PRIVATE void sqlite3BtreeCursorList(Btree*);
#endif

#ifndef SQLITE_OMIT_WAL
SQLITE_PRIVATE   int sqlite3BtreeCheckpoint(Btree*, int, int *, int *);
#endif

/*
** If we are not using shared cache, then there is no need to
** use mutexes to access the BtShared structures.  So make the
** Enter and Leave procedures no-ops.
*/
#ifndef SQLITE_OMIT_SHARED_CACHE
SQLITE_PRIVATE   void sqlite3BtreeEnter(Btree*);
SQLITE_PRIVATE   void sqlite3BtreeEnterAll(sqlite3*);
#else
# define sqlite3BtreeEnter(X) 
# define sqlite3BtreeEnterAll(X)
#endif

#if !defined(SQLITE_OMIT_SHARED_CACHE) && SQLITE_THREADSAFE
SQLITE_PRIVATE   int sqlite3BtreeSharable(Btree*);
SQLITE_PRIVATE   void sqlite3BtreeLeave(Btree*);
SQLITE_PRIVATE   void sqlite3BtreeEnterCursor(BtCursor*);
SQLITE_PRIVATE   void sqlite3BtreeLeaveCursor(BtCursor*);
SQLITE_PRIVATE   void sqlite3BtreeLeaveAll(sqlite3*);
#ifndef NDEBUG
  /* These routines are used inside assert() statements only. */
SQLITE_PRIVATE   int sqlite3BtreeHoldsMutex(Btree*);
SQLITE_PRIVATE   int sqlite3BtreeHoldsAllMutexes(sqlite3*);
SQLITE_PRIVATE   int sqlite3SchemaMutexHeld(sqlite3*,int,Schema*);
#endif
#else

# define sqlite3BtreeSharable(X) 0
# define sqlite3BtreeLeave(X)
# define sqlite3BtreeEnterCursor(X)
# define sqlite3BtreeLeaveCursor(X)
# define sqlite3BtreeLeaveAll(X)

# define sqlite3BtreeHoldsMutex(X) 1
# define sqlite3BtreeHoldsAllMutexes(X) 1
# define sqlite3SchemaMutexHeld(X,Y,Z) 1
#endif


#endif /* _BTREE_H_ */

/************** End of btree.h ***********************************************/
