/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/dirent.c
 * \brief   Directory entry handling
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
#include "base/errors.h"
#include "file.h"
#include "base/mem.h"
#include "base/petasc.h"

#include "dirent.h"

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

