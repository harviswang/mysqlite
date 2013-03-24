/************** Begin file fts3.h ********************************************/
/*
** 2006 Oct 10
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This header file is used by programs that want to link against the
** FTS3 library.  All it does is declare the sqlite3Fts3Init() interface.
*/

#if 0
extern "C" {
#endif  /* __cplusplus */

SQLITE_PRIVATE int sqlite3Fts3Init(sqlite3 *db);

#if 0
}  /* extern "C" */
#endif  /* __cplusplus */

/************** End of fts3.h ************************************************/
