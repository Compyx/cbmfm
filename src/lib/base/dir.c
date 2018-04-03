/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/dir.c
 * \brief   Directory handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 *  CbmFM - a file manager for CBM 8-bit emulation files
 *  Copyright (C) 2018  Bas Wassink <b.wassink@ziggo.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "cbmfm_types.h"
#include "base.h"

#include "dir.h"


/** \brief  Initial size of array of dirent pointers
 */
#define DIR_ENTRY_COUNT_INIT    512


static const char *filetype_str[] = {
    "del", "seq", "prg", "usr", "rel", "???", "???", "???"
};


void cbmfm_block_init(cbmfm_block_t *block)
{
    block->track = -1;
    block->sector = -1;
    block->data = NULL;
}


/** \brief  Get CBMDOS filetype string
 *
 * \param[in]   filetype    CBMDOS filetype byte
 *
 * \return  0-terminated file type in ASCII
 */
const char *cbmfm_cbmdos_filetype(uint8_t filetype)
{
    return filetype_str[filetype & 0x07];
}



/** \brief  Allocate a dirent object
 *
 * \return  dirent object
 */
cbmfm_dirent_t *cbmfm_dirent_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_dirent_t));
}


/** \brief  Initialize \a dirent to usable state
 *
 * Sets all members of \a dirent to 0/NULL, zero out CBMDOS filename
 *
 *
 * \param[in,out]   dirent  dirent object
 */
void cbmfm_dirent_init(cbmfm_dirent_t *dirent)
{
    memset(dirent->filename, 0, CBMFM_CBMDOS_FILENAME_LEN);
    dirent->filedata = NULL;
    dirent->filesize = 0;
    dirent->filetype = 0;
    cbmfm_block_init(&(dirent->first_block));
    dirent->size_blocks = 0;

    dirent->dir = NULL;
    dirent->image = NULL;
}


/** \brief  Allocate and initialize a dirent object
 *
 * \return  dirent object
 */
cbmfm_dirent_t *cbmfm_dirent_new(void)
{
    cbmfm_dirent_t *dirent = cbmfm_dirent_alloc();
    cbmfm_dirent_init(dirent);
    return dirent;
}


/** \brief  Create a deep copy of \a dirent
 *
 * \param[in]   dirent  dirent object
 *
 * \return  deep copy of \a dirent
 */
cbmfm_dirent_t *cbmfm_dirent_dup(const cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_t *dupl = cbmfm_dirent_new();

    /* copy file name */
    memcpy(dupl->filename, dirent->filename, CBMFM_CBMDOS_FILENAME_LEN);
    if (dirent->filedata != NULL) {
        /* copy file data */
        dupl->filedata = cbmfm_memdup(dirent->filedata, dirent->filesize);
    } else {
        dupl->filedata = NULL;
    }
    dupl->filesize = dirent->filesize;
    dupl->filetype = dirent->filetype;
    dupl->first_block = dirent->first_block;
    dupl->size_blocks = dirent->size_blocks;

    dupl->dir = dirent->dir;
    dupl->image = dirent->image;

    return dupl;
}


/** \brief  Clean up data used by members of \a dirent
 *
 * Cleans up \a dirent and resets it for reuse
 *
 * \param[in,out]   dirent
 */
void cbmfm_dirent_cleanup(cbmfm_dirent_t *dirent)
{
    if (dirent->filedata != NULL) {
        cbmfm_free(dirent->filedata);
    }

    /* the dir and image members are references, we don't free those */

    cbmfm_dirent_init(dirent);
}


/** \brief  Free \a dirent and its members
 *
 * \param[in,out]   dirent
 */
void cbmfm_dirent_free(cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_cleanup(dirent);
    cbmfm_free(dirent);
}


/** \brief  Dump \a dirent on stdout as a directory listing line
 *
 * \param[in]   dirent  dirent object
 *
 * \return  number of chars dumped on stdout
 */
int cbmfm_dirent_dump(const cbmfm_dirent_t *dirent)
{
    char name[CBMFM_CBMDOS_FILENAME_LEN + 1];

    cbmfm_pet_to_asc_str(name, dirent->filename, CBMFM_CBMDOS_FILENAME_LEN);

    return printf("%-5u \"%s\" %c%s%c\n",
            dirent->size_blocks, name,
            dirent->filetype & CBMFM_CBMDOS_FILE_CLOSED_BIT ? ' ' : '*',
            cbmfm_cbmdos_filetype(dirent->filetype),
            dirent->filetype & CBMFM_CBMDOS_FILE_LOCKED_BIT ? '<' : ' ');
}

/*
 * cbmfm_dir_t methods
 */

/** \brief  Initialize \a dir to a usable state
 *
 * Allocates an array of #DIR_ENTRY_COUNT #cbmfm_dirent_t pointers
 */
void cbmfm_dir_init(cbmfm_dir_t *dir)
{
    dir->entries = cbmfm_malloc(DIR_ENTRY_COUNT_INIT * sizeof *(dir->entries));
    dir->entry_max = DIR_ENTRY_COUNT_INIT;
    dir->entry_used = 0;
    dir->image = NULL;
}


/** \brief  Allocate a dir object
 *
 * \return  dir object
 */
cbmfm_dir_t *cbmfm_dir_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_dir_t));
}


/** \brief  Allocate and initialize a new dir object
 *
 * \return  dir object
 */
cbmfm_dir_t *cbmfm_dir_new(void)
{
    cbmfm_dir_t *dir = cbmfm_dir_alloc();
    cbmfm_dir_init(dir);
    return dir;
}


/** \brief  Clean members of \a dir but not \a dir itself
 *
 * \param[in,out]   dir dir object
 */
void cbmfm_dir_cleanup(cbmfm_dir_t *dir)
{
    size_t i;

    for (i = 0; i < dir->entry_used; i++) {
        cbmfm_dirent_free(dir->entries[i]);
    }
    cbmfm_free(dir->entries);
}


/** \brief  Free \a dir and its members
 *
 * \param[in,out]   dir dir object
 */
void cbmfm_dir_free(cbmfm_dir_t *dir)
{
    cbmfm_dir_cleanup(dir);
    cbmfm_free(dir);
}


/** \brief  Append a deep copy of \a dirent to \a dir
 *
 * Creates a deep copy of \a dirent and appends it to \a dir.
 *
 * \param[in,out]   dir     dir object
 * \param[in]       dirent  dirent object
 */
void cbmfm_dir_append_dirent(cbmfm_dir_t *dir, const cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_t *dupl;

    /* resize array? */
    if (dir->entry_max == dir->entry_used) {
        dir->entries = cbmfm_realloc(dir->entries,
                dir->entry_max * 2 * sizeof *(dir->entries));
        dir->entry_max *= 2;
    }

    /* duplicate dirent */
    dupl = cbmfm_dirent_dup(dirent);
    /* store reference to parent dir */
    dupl->dir = dir;

    /* add to listing */
    dir->entries[dir->entry_used++] = dupl;
}


/** \brief  Dump \a dir on stdout like a 1541 directory listing
 *
 * \param[in]   dir     directory object
 */
void cbmfm_dir_dump(const cbmfm_dir_t *dir)
{
    size_t index;

    for (index = 0; index < dir->entry_used; index++) {
        cbmfm_dirent_dump(dir->entries[index]);
    }
}
