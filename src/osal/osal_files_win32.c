/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus-core - osal_files_win32.c                                 *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2009 Richard Goedeken                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* This file contains the definitions for the unix-specific file handling
 * functions
 */

#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>

#include "osal_files.h"

/* global functions */

#ifdef __cplusplus
extern "C"{
#endif

EXPORT int CALL osal_path_exists(const char *path)
{
    struct _stat fileinfo;
    return _stat(path, &fileinfo) == 0 ? 1 : 0;
}

EXPORT int CALL osal_is_absolute_path(const char* name)
{
    return strchr(name, ':') != NULL || name[0] == '\\' || name[0] == '/';
}

EXPORT int CALL osal_is_directory(const char* _name)
{
    char DirName[MAX_PATH + 1];
    int namelen = 0;
    /* we must remove any trailing backslash on the end of the pathname, or this will fail */
    strncpy(DirName, _name, MAX_PATH);
    DirName[MAX_PATH] = 0;
    namelen = strlen(DirName);
    if (namelen > 0 && DirName[namelen - 1] == OSAL_DIR_SEPARATOR_CHAR)
        DirName[namelen - 1] = '\0';
    return (GetFileAttributesA(DirName) & FILE_ATTRIBUTE_DIRECTORY);
}

EXPORT int CALL osal_mkdirp(const char* dirpath)
{
    struct _stat fileinfo;
    size_t dirpathlen = strlen(dirpath);
    char* currpath = _strdup(dirpath);

    /* first, remove sub-dirs on the end (by replacing slashes with NULL chars) until we find an existing directory */
    while (strlen(currpath) > 1 && _stat(currpath, &fileinfo) != 0) {
        char* lastslash = strrchr(currpath, OSAL_DIR_SEPARATOR_CHAR);
        if (lastslash == NULL) {
            free(currpath);
            return 1; /* error: we never found an existing directory, this path is bad */
        }
        *lastslash = 0;
    }

    /* then walk up the path chain, creating directories along the way */
    do {
        if (currpath[strlen(currpath) - 1] != OSAL_DIR_SEPARATOR_CHAR && _stat(currpath, &fileinfo) != 0)
        {
            if (_mkdir(currpath) != 0)
            {
                free(currpath);
                return 2;        /* mkdir failed */
            }
        }
        if (strlen(currpath) == dirpathlen)
            break;
        else
            currpath[strlen(currpath)] = OSAL_DIR_SEPARATOR_CHAR;
    } while (1);

    free(currpath);
    return 0;
}

typedef struct {
    HANDLE hFind;
    WIN32_FIND_DATAA find_data;
} dir_search_info;

EXPORT void * CALL osal_search_dir_open(const char *pathname)
{
    char SearchString[MAX_PATH + 1];
    dir_search_info *pInfo = (dir_search_info *)malloc(sizeof(dir_search_info));

    if (pInfo == NULL)
        return NULL;

    pInfo->hFind = INVALID_HANDLE_VALUE;
    pInfo->find_data.cFileName[0] = 0;

	if (pathname[strlen(pathname) - 1] == OSAL_DIR_SEPARATOR_CHAR)
		snprintf(SearchString, MAX_PATH, "%s*", pathname);
    else
		snprintf(SearchString, MAX_PATH, "%s%s*", pathname, OSAL_DIR_SEPARATOR_STR);
    SearchString[MAX_PATH] = 0;

    pInfo->hFind = FindFirstFileA(SearchString, &pInfo->find_data);
    return (void *) pInfo;
}

EXPORT const char * CALL osal_search_dir_read_next(void * search_info)
{
    static char last_filename[_MAX_PATH];
    dir_search_info *pInfo = (dir_search_info *) search_info;

    if (pInfo == NULL || pInfo->hFind == INVALID_HANDLE_VALUE || pInfo->find_data.cFileName[0] == 0)
        return NULL;

	strcpy(last_filename, pInfo->find_data.cFileName);

    if (FindNextFileA(pInfo->hFind, &pInfo->find_data) == 0)
    {
        pInfo->find_data.cFileName[0] = 0;
    }

    return last_filename;
}

EXPORT void CALL osal_search_dir_close(void * search_info)
{
    dir_search_info *pInfo = (dir_search_info *) search_info;

    if (pInfo != NULL)
    {
        if (pInfo->hFind != INVALID_HANDLE_VALUE)
            FindClose(pInfo->hFind);
        free(pInfo);
    }
}

#ifdef __cplusplus
}
#endif
