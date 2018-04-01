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


void cbmfm_block_init(cbmfm_block_t *block)
{
    block->track = -1;
    block->sector = -1;
    block->data = NULL;
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
}


cbmfm_dir_t *cbmfm_dir_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_dir_t));
}


cbmfm_dir_t *cbmfm_dir_new(void)
{
    cbmfm_dir_t *dir = cbmfm_dir_alloc();
    cbmfm_dir_init(dir);
    return dir;
}


void cbmfm_dir_cleanup(cbmfm_dir_t *dir)
{
    size_t i;

    for (i = 0; i < dir->entry_used; i++) {
        cbmfm_dirent_free(dir->entries[i]);
    }
    cbmfm_free(dir->entries);
}


void cbmfm_dir_free(cbmfm_dir_t *dir)
{
    cbmfm_dir_cleanup(dir);
    cbmfm_free(dir);
}
