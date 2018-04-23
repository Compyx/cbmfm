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
#include "base/dirent.h"
#include "base/errors.h"
#include "base/mem.h"
#include "base/image.h"

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


/** \brief  Get number of available blocks for \a track in \a image
 *
 * \param[in]   image   dxx image
 * \param[in]   track   track number
 *
 * \return  block count of \a track or -1 on error
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 */
int cbmfm_dxx_track_block_count(cbmfm_dxx_image_t *image, int track)
{
    const cbmfm_dxx_speedzone_t *zones = image->zones;
    int z = 0;

    if (track < 1 || track > image->track_max) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_TRACK;
        return -1;
    }

    while (track > zones[z].trk_hi && zones[z].trk_hi >= 1) {
        z++;
    }
    return zones[z].blocks;
}


/** \brief  Initialize Dxx image block iterator
 *
 * \param[out]  iter    block iterator
 * \param[in]   image   dxx image
 * \param[in]   track   track number of initial block
 * \param[in]   sector  sector number of initial block
 *
 * \return  true if succesful
 *
 * \throw   #CBMFM_ERR_ILLEGAL_TRACK
 * \throw   #CBMFM_ERR_ILLEGAL_SECTOR
 */
bool cbmfm_dxx_block_iter_init(cbmfm_dxx_block_iter_t *iter,
                               cbmfm_dxx_image_t *image,
                               int track, int sector)
{
    iter->image = image;

    cbmfm_block_init(&(iter->curr));
    cbmfm_block_init(&(iter->prev));

    /* check track number */
    if (track < 1 || track > image->track_max) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_TRACK;
        return false;
    }
    /* check sector number */
    if (sector < 0 || sector >= cbmfm_dxx_track_block_count(image, track)) {
        cbmfm_errno = CBMFM_ERR_ILLEGAL_SECTOR;
        return false;
    }

    iter->curr.track = track;
    iter->curr.sector = sector;
    return true;
}


/** \brief  Move block iterator \a iter to the next block
 *
 * \param[in,out]   iter    dxx block iterator
 *
 * \return  true if next block found
 */
bool cbmfm_dxx_block_iter_next(cbmfm_dxx_block_iter_t *iter)
{
    intmax_t offset;
    uint8_t *data;
    uint8_t next_track;
    uint8_t next_sector;

    if (iter->curr.track == 0) {
        return false;
    }

    offset = cbmfm_dxx_block_offset(iter->image->zones,
            iter->curr.track, iter->curr.sector);
    data = iter->image->data + offset;
    next_track = data[0];
    next_sector = data[1];

    iter->prev.track = iter->curr.track;
    iter->prev.sector = iter->curr.sector;
    iter->curr.track = next_track;
    iter->curr.sector = next_sector;

    return true;
}


/** \brief  Copy 256 bytes of current block in \a iter to \a dest
 *
 * \param[in]   iter    block iterator
 * \param[out]  dest    destination of block data (should be 256+ bytes)
 */
void cbmfm_dxx_block_iter_read_data(cbmfm_dxx_block_iter_t *iter, uint8_t *dest)
{
    intmax_t offset;
    uint8_t *data;

    offset = cbmfm_dxx_block_offset(iter->image->zones,
            iter->curr.track, iter->curr.sector);
    data = iter->image->data + offset;

    memcpy(dest, data, CBMFM_BLOCK_SIZE_RAW);
}


/** \brief  Write data to current block
 *
 * \param[in,out]   iter    Dxx block iterator
 * \param[in]       data    data to write to block
 * \param[in]       size    number of bytes to write
 */
void cbmfm_dxx_block_iter_write_data(cbmfm_dxx_block_iter_t *iter,
                                     const uint8_t *data,
                                     size_t size)
{
    intmax_t offset;

    offset = cbmfm_dxx_block_offset(iter->image->zones,
            iter->curr.track, iter->curr.sector);
    memcpy(iter->image->data + offset, data, size);
}


/** \brief  Initialize Dxx dirent object
 *
 * \param[out]  dirent  directory entry
 * \param[in]   type    Dxx image type enum
 */
void cbmfm_dxx_dirent_init(cbmfm_dirent_t *dirent, int type)
{
    cbmfm_dirent_dxx_t *extra= &(dirent->extra.dxx);

    cbmfm_dirent_init(dirent);
    cbmfm_block_init(&(extra->first_block));
    dirent->image_type = type;
}
