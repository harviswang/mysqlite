/************** Begin file sqliteicu.h ***************************************/
/*
** 2008 May 26
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
** ICU extension.  All it does is declare the sqlite3IcuInit() interface.
*/

#if 0
extern "C" {
#endif  /* __cplusplus */

SQLITE_PRIVATE int sqlite3IcuInit(sqlite3 *db);

#if 0
}  /* extern "C" */
#endif  /* __cplusplus */


/************** End of sqliteicu.h *******************************************/
