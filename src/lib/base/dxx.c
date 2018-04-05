/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/dxx.c
 * \brief   Generic disk image functions
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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "cbmfm_types.h"
#include "base/mem.h"
#include "base/errors.h"

#include "dxx.h"


/** \brief  Get block number of block (\a track, \a sector)
 *
 * \param[in]   zones   speed zone table
 * \param[in]   track   track number
 * \param[in]   sector  sector number
 *
 * \return  block number (starting at 0) or -1 on error
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 * \throw   #CBMFM_ERR_ILLEGAL_SECTOR
 */
int cbmfm_dxx_block_number(const cbmfm_dxx_speedzone_t *zones,
                           int track, int sector)
{
    int blocks = 0;
    int z = 0;

    if (track < 1) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_TRACK;
        return -1;
    }
    if (sector < 0) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_SECTOR;
        return -1;
    }

    while (zones[z].trk_lo >= 1) {
        if (sector >= zones[z].blocks) {
            /* sector# too high */
            cbmfm_errno = CBMFM_ERR_ILLEGAL_SECTOR;
            return -1;
        }

        if (track >= zones[z].trk_lo && track <= zones[z].trk_hi) {
            /* in the zone :) */
            return blocks + (track - zones[z].trk_lo) * zones[z].blocks + sector;
        } else {
            /* add size of zone */
            blocks += ((zones[z].trk_hi - zones[z].trk_lo + 1)
                    * zones[z].blocks);
        }

        z++;
    }

    /* ran out of zones -> track# too high */
    cbmfm_errno = CBMFM_ERR_ILLEGAL_TRACK;
    return -1;
}


/** \brief  Get offset of block (\a track, \a sector) in bytes
 *
 * \param[in]   zones   speed zone table
 * \param[in]   track   track number
 * \param[in]   sector  sector number
 *
 * \return  offset in an image for (\a track, \a sector) or -1 on error
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 * \throw   #CBMFM_ERR_ILLEGAL_SECTOR
 */
intmax_t cbmfm_dxx_block_offset(const cbmfm_dxx_speedzone_t *zones,
                                int track, int sector)
{
    int blocks = cbmfm_dxx_block_number(zones, track, sector);
    if (blocks < 0) {
        return -1;
    }
    return blocks * CBMFM_BLOCK_SIZE_RAW;
}


/** \brief  Read and allocate 256 bytes of  (\a track,\a sector) into \a block
 *
 * \param[in,out]   block   block object
 * \param[in]       image   dxx image
 * \param[in]       track   track number
 * \param[in]       sector  sector number
 *
 * \return  true on success
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 * \throw   #CBMFM_ERR_ILLEGAL_SECTOR
 */
bool cbmfm_dxx_block_read(cbmfm_block_t *block,
                          cbmfm_dxx_image_t *image,
                          int track, int sector)
{
    intmax_t offset;
    const cbmfm_dxx_speedzone_t *zones = image->zones;

    offset = cbmfm_dxx_block_offset(zones, track, sector);
    if (offset < 0) {
        return false;
    }

    block->track = track;
    block->sector = sector;
    block->data = cbmfm_memdup(image->data + offset, CBMFM_BLOCK_SIZE_RAW);
    return true;
}


/** \brief  Free block data inside \a block
 *
 * Frees the 256 bytes inside \a block
 *
 * \param[in,out]   block   block object
 *
 * \todo    move to more generic file
 */
void cbmfm_dxx_block_cleanup(cbmfm_block_t *block)
{
    cbmfm_free(block->data);
    block->data = NULL;
}


void cbmfm_dxx_block_dump(const cbmfm_block_t *block)
{
    int row;
    int col;

    for (row = 0; row < 16; row++) {
        printf("%02d:%02d:%02x: ", block->track, block->sector, row * 16);
        for (col = 0; col < 16; col++) {
            printf("%02x ", block->data[row * 16 + col]);
        }
        for (col = 0; col < 16; col++) {
            int ch = block->data[row * 16 + col];
            putchar(isprint(ch) ? ch : '.');
        }
        putchar('\n');
    }
}
