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
#include <inttypes.h>
#include <stdbool.h>

#include "cbmfm_types.h"
#include "base/dirent.h"
#include "base/dxx.h"
#include "base/errors.h"
#include "base/file.h"
#include "base/mem.h"
#include "base/petasc.h"

#include "dir.h"


/** \brief  Initial size of array of dirent pointers
 */
#define DIR_ENTRY_COUNT_INIT    512




/*
 * cbmfm_dir_t methods
 */

/** \brief  Initialize \a dir to a usable state
 *
 * Allocates an array of #DIR_ENTRY_COUNT_INIT #cbmfm_dirent_t pointers
 *
 * \param[in,out]   dir directory object
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


/** \brief  Initialize directory iterator
 *
 * \param[out]  iter    directory iterator
 * \param[in]   image   Dxx image
 * \param[in]   track   track number of first directory block
 * \param[in]   sector  sector number of first directory block
 *
 * \return  bool
 */
bool cbmfm_dxx_dir_iter_init(cbmfm_dxx_dir_iter_t *iter,
                         cbmfm_dxx_image_t *image,
                         int track, int sector)
{
    if (!cbmfm_dxx_block_iter_init(&(iter->block_iter), image, track, sector)) {
        return false;
    }
    iter->image = image;
    iter->entry_offset = 0;
    return true;
}


/** \brief  Move directory iterator to next entry
 *
 * \param[in,out]   iter    directory iterator
 *
 * \return  true if a next entry was found, false otherwise
 */
bool cbmfm_dxx_dir_iter_next(cbmfm_dxx_dir_iter_t *iter)
{
    uint8_t *entry;

    iter->entry_offset += CBMFM_DXX_DIRENT_SIZE;

    if (iter->entry_offset >= CBMFM_BLOCK_SIZE_RAW) {
        /* move block iterator to next block */
        if (!cbmfm_dxx_block_iter_next(&(iter->block_iter))) {
            printf("END OF BLOCK ITER\n");
            return false;
        }
        iter->entry_offset = 0;
    }

    entry = cbmfm_dxx_dir_iter_entry_ptr(iter);
    if (entry[CBMFM_D64_DIRENT_FILE_NAME] == 0x00) {
        /* no more entries */
        return false;
    }
    return true;
}


/** \brief  Get a pointer to the current directory entry
 *
 * \param[in]   iter    directory iterator
 *
 * \return  pointer to entry on image
 */
uint8_t *cbmfm_dxx_dir_iter_entry_ptr(cbmfm_dxx_dir_iter_t *iter)
{
    return iter->image->data +
        cbmfm_dxx_block_offset(iter->image->zones,
                iter->block_iter.curr.track,
                iter->block_iter.curr.sector) +
        iter->entry_offset;
}
