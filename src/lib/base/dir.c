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
#include "base/dxx.h"
#include "base/file.h"
#include "base.h"

#include "dir.h"


/** \brief  Initial size of array of dirent pointers
 */
#define DIR_ENTRY_COUNT_INIT    512


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
 * \param[in,out]   dirent  dirent object
 */
void cbmfm_dirent_init(cbmfm_dirent_t *dirent)
{
    memset(dirent->filename, 0, CBMFM_CBMDOS_FILE_NAME_LEN);
    dirent->filedata = NULL;
    dirent->filesize = 0;
    dirent->filetype = 0;
    dirent->size_blocks = 0;

    dirent->index = 0;
    dirent->image_type = CBMFM_IMAGE_TYPE_INVALID;

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
    memcpy(dupl->filename, dirent->filename, CBMFM_CBMDOS_FILE_NAME_LEN);
    if (dirent->filedata != NULL) {
        /* copy file data */
        dupl->filedata = cbmfm_memdup(dirent->filedata, dirent->filesize);
    } else {
        dupl->filedata = NULL;
    }
    dupl->filesize = dirent->filesize;
    dupl->filetype = dirent->filetype;
    dupl->size_blocks = dirent->size_blocks;

    dupl->dir = dirent->dir;
    dupl->image = dirent->image;
    dupl->image_type = dirent->image_type;
    dupl->index = dirent->index;

    /* copy image-type dependent data */
    switch (dirent->image_type) {
        case CBMFM_IMAGE_TYPE_D64:  /* fall through */
        case CBMFM_IMAGE_TYPE_D71:  /* fall through */
        case CBMFM_IMAGE_TYPE_D80:  /* fall through */
        case CBMFM_IMAGE_TYPE_D81:  /* fall through */
        case CBMFM_IMAGE_TYPE_D82:
            dupl->extra.dxx = dirent->extra.dxx;
            break;
        case CBMFM_IMAGE_TYPE_T64:
            dupl->extra.t64 = dirent->extra.t64;
            break;
        default:
            break;
    }

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
    int x;

    char name[CBMFM_CBMDOS_FILE_NAME_LEN + 1];

    cbmfm_pet_to_asc_str(name, dirent->filename, CBMFM_CBMDOS_FILE_NAME_LEN);

    x = printf("%-5u \"%s\" %c%s%c  [%03" PRIx16 "]",
            dirent->size_blocks, name,
            dirent->filetype & CBMFM_CBMDOS_FILE_CLOSED_BIT ? ' ' : '*',
            cbmfm_cbmdos_filetype(dirent->filetype),
            dirent->filetype & CBMFM_CBMDOS_FILE_LOCKED_BIT ? '<' : ' ',
            dirent->index);

    switch (dirent->image_type) {
        case CBMFM_IMAGE_TYPE_D64:  /* fall through */
        case CBMFM_IMAGE_TYPE_D71:  /* fall through */
        case CBMFM_IMAGE_TYPE_D80:  /* fall through */
        case CBMFM_IMAGE_TYPE_D81:  /* fall through */
        case CBMFM_IMAGE_TYPE_D82:
            return x + printf(" (%2d,%2d)\n",
                    dirent->extra.dxx.first_block.track,
                    dirent->extra.dxx.first_block.sector);
        case CBMFM_IMAGE_TYPE_T64:
            return x + printf(" ($%" PRIx16 "-$%" PRIx16 "), offset $%" PRIx32 "\n",
                    dirent->extra.t64.load_addr,
                    dirent->extra.t64.end_addr,
                    dirent->extra.t64.data_offset);
        default:
            return x + printf("\n");
    }
}


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


/** \brief  Calculate number of blocks from \a size
 *
 * Calculate the number of blocks a file of \a size bytes would occupy on a
 * floppy disk.
 *
 * \param[in]   size    size in bytes
 *
 * \return  size in blocks
 */
uint16_t cbmfm_size_to_blocks(size_t size)
{
    return (uint16_t)(size / CBMFM_BLOCK_SIZE_DATA
            + (size % CBMFM_BLOCK_SIZE_DATA == 0 ? 0 : 1));
}
