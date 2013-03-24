/************** Begin file pager.h *******************************************/
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
** This header file defines the interface that the sqlite page cache
** subsystem.  The page cache subsystem reads and writes a file a page
** at a time and provides a journal for rollback.
*/

#ifndef _PAGER_H_
#define _PAGER_H_

/*
** Default maximum size for persistent journal files. A negative 
** value means no limit. This value may be overridden using the 
** sqlite3PagerJournalSizeLimit() API. See also "PRAGMA journal_size_limit".
*/
#ifndef SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT
  #define SQLITE_DEFAULT_JOURNAL_SIZE_LIMIT -1
#endif

/*
** The type used to represent a page number.  The first page in a file
** is called page 1.  0 is used to represent "not a page".
*/
typedef u32 Pgno;

/*
** Each open file is managed by a separate instance of the "Pager" structure.
*/
typedef struct Pager Pager;

/*
** Handle type for pages.
*/
typedef struct PgHdr DbPage;

/*
** Page number PAGER_MJ_PGNO is never used in an SQLite database (it is
** reserved for working around a windows/posix incompatibility). It is
** used in the journal to signify that the remainder of the journal file 
** is devoted to storing a master journal name - there are no more pages to
** roll back. See comments for function writeMasterJournal() in pager.c 
** for details.
*/
#define PAGER_MJ_PGNO(x) ((Pgno)((PENDING_BYTE/((x)->pageSize))+1))

/*
** Allowed values for the flags parameter to sqlite3PagerOpen().
**
** NOTE: These values must match the corresponding BTREE_ values in btree.h.
*/
#define PAGER_OMIT_JOURNAL  0x0001    /* Do not use a rollback journal */
#define PAGER_MEMORY        0x0002    /* In-memory database */

/*
** Valid values for the second argument to sqlite3PagerLockingMode().
*/
#define PAGER_LOCKINGMODE_QUERY      -1
#define PAGER_LOCKINGMODE_NORMAL      0
#define PAGER_LOCKINGMODE_EXCLUSIVE   1

/*
** Numeric constants that encode the journalmode.  
*/
#define PAGER_JOURNALMODE_QUERY     (-1)  /* Query the value of journalmode */
#define PAGER_JOURNALMODE_DELETE      0   /* Commit by deleting journal file */
#define PAGER_JOURNALMODE_PERSIST     1   /* Commit by zeroing journal header */
#define PAGER_JOURNALMODE_OFF         2   /* Journal omitted.  */
#define PAGER_JOURNALMODE_TRUNCATE    3   /* Commit by truncating journal */
#define PAGER_JOURNALMODE_MEMORY      4   /* In-memory journal file */
#define PAGER_JOURNALMODE_WAL         5   /* Use write-ahead logging */

/*
** The remainder of this file contains the declarations of the functions
** that make up the Pager sub-system API. See source code comments for 
** a detailed description of each routine.
*/

/* Open and close a Pager connection. */ 
SQLITE_PRIVATE int sqlite3PagerOpen(
  sqlite3_vfs*,
  Pager **ppPager,
  const char*,
  int,
  int,
  int,
  void(*)(DbPage*)
);
SQLITE_PRIVATE int sqlite3PagerClose(Pager *pPager);
SQLITE_PRIVATE int sqlite3PagerReadFileheader(Pager*, int, unsigned char*);

/* Functions used to configure a Pager object. */
SQLITE_PRIVATE void sqlite3PagerSetBusyhandler(Pager*, int(*)(void *), void *);
SQLITE_PRIVATE int sqlite3PagerSetPagesize(Pager*, u32*, int);
SQLITE_PRIVATE int sqlite3PagerMaxPageCount(Pager*, int);
SQLITE_PRIVATE void sqlite3PagerSetCachesize(Pager*, int);
SQLITE_PRIVATE void sqlite3PagerShrink(Pager*);
SQLITE_PRIVATE void sqlite3PagerSetSafetyLevel(Pager*,int,int,int);
SQLITE_PRIVATE int sqlite3PagerLockingMode(Pager *, int);
SQLITE_PRIVATE int sqlite3PagerSetJournalMode(Pager *, int);
SQLITE_PRIVATE int sqlite3PagerGetJournalMode(Pager*);
SQLITE_PRIVATE int sqlite3PagerOkToChangeJournalMode(Pager*);
SQLITE_PRIVATE i64 sqlite3PagerJournalSizeLimit(Pager *, i64);
SQLITE_PRIVATE sqlite3_backup **sqlite3PagerBackupPtr(Pager*);

/* Functions used to obtain and release page references. */ 
SQLITE_PRIVATE int sqlite3PagerAcquire(Pager *pPager, Pgno pgno, DbPage **ppPage, int clrFlag);
#define sqlite3PagerGet(A,B,C) sqlite3PagerAcquire(A,B,C,0)
SQLITE_PRIVATE DbPage *sqlite3PagerLookup(Pager *pPager, Pgno pgno);
SQLITE_PRIVATE void sqlite3PagerRef(DbPage*);
SQLITE_PRIVATE void sqlite3PagerUnref(DbPage*);

/* Operations on page references. */
SQLITE_PRIVATE int sqlite3PagerWrite(DbPage*);
SQLITE_PRIVATE void sqlite3PagerDontWrite(DbPage*);
SQLITE_PRIVATE int sqlite3PagerMovepage(Pager*,DbPage*,Pgno,int);
SQLITE_PRIVATE int sqlite3PagerPageRefcount(DbPage*);
SQLITE_PRIVATE void *sqlite3PagerGetData(DbPage *); 
SQLITE_PRIVATE void *sqlite3PagerGetExtra(DbPage *); 

/* Functions used to manage pager transactions and savepoints. */
SQLITE_PRIVATE void sqlite3PagerPagecount(Pager*, int*);
SQLITE_PRIVATE int sqlite3PagerBegin(Pager*, int exFlag, int);
SQLITE_PRIVATE int sqlite3PagerCommitPhaseOne(Pager*,const char *zMaster, int);
SQLITE_PRIVATE int sqlite3PagerExclusiveLock(Pager*);
SQLITE_PRIVATE int sqlite3PagerSync(Pager *pPager);
SQLITE_PRIVATE int sqlite3PagerCommitPhaseTwo(Pager*);
SQLITE_PRIVATE int sqlite3PagerRollback(Pager*);
SQLITE_PRIVATE int sqlite3PagerOpenSavepoint(Pager *pPager, int n);
SQLITE_PRIVATE int sqlite3PagerSavepoint(Pager *pPager, int op, int iSavepoint);
SQLITE_PRIVATE int sqlite3PagerSharedLock(Pager *pPager);

#ifndef SQLITE_OMIT_WAL
SQLITE_PRIVATE   int sqlite3PagerCheckpoint(Pager *pPager, int, int*, int*);
SQLITE_PRIVATE   int sqlite3PagerWalSupported(Pager *pPager);
SQLITE_PRIVATE   int sqlite3PagerWalCallback(Pager *pPager);
SQLITE_PRIVATE   int sqlite3PagerOpenWal(Pager *pPager, int *pisOpen);
SQLITE_PRIVATE   int sqlite3PagerCloseWal(Pager *pPager);
#endif

#ifdef SQLITE_ENABLE_ZIPVFS
SQLITE_PRIVATE   int sqlite3PagerWalFramesize(Pager *pPager);
#endif

/* Functions used to query pager state and configuration. */
SQLITE_PRIVATE u8 sqlite3PagerIsreadonly(Pager*);
SQLITE_PRIVATE int sqlite3PagerRefcount(Pager*);
SQLITE_PRIVATE int sqlite3PagerMemUsed(Pager*);
SQLITE_PRIVATE const char *sqlite3PagerFilename(Pager*, int);
SQLITE_PRIVATE const sqlite3_vfs *sqlite3PagerVfs(Pager*);
SQLITE_PRIVATE sqlite3_file *sqlite3PagerFile(Pager*);
SQLITE_PRIVATE const char *sqlite3PagerJournalname(Pager*);
SQLITE_PRIVATE int sqlite3PagerNosync(Pager*);
SQLITE_PRIVATE void *sqlite3PagerTempSpace(Pager*);
SQLITE_PRIVATE int sqlite3PagerIsMemdb(Pager*);
SQLITE_PRIVATE void sqlite3PagerCacheStat(Pager *, int, int, int *);
SQLITE_PRIVATE void sqlite3PagerClearCache(Pager *);
SQLITE_PRIVATE int sqlite3SectorSize(sqlite3_file *);

/* Functions used to truncate the database file. */
SQLITE_PRIVATE void sqlite3PagerTruncateImage(Pager*,Pgno);

#if defined(SQLITE_HAS_CODEC) && !defined(SQLITE_OMIT_WAL)
SQLITE_PRIVATE void *sqlite3PagerCodec(DbPage *);
#endif

/* Functions to support testing and debugging. */
#if !defined(NDEBUG) || defined(SQLITE_TEST)
SQLITE_PRIVATE   Pgno sqlite3PagerPagenumber(DbPage*);
SQLITE_PRIVATE   int sqlite3PagerIswriteable(DbPage*);
#endif
#ifdef SQLITE_TEST
SQLITE_PRIVATE   int *sqlite3PagerStats(Pager*);
SQLITE_PRIVATE   void sqlite3PagerRefdump(Pager*);
  void disable_simulated_io_errors(void);
  void enable_simulated_io_errors(void);
#else
# define disable_simulated_io_errors()
# define enable_simulated_io_errors()
#endif

#endif /* _PAGER_H_ */

/************** End of pager.h ***********************************************/
