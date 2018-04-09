/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/dxx.h
 * \brief   Generic disk image functions - header
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

#ifndef CBMFM_LIB_BASE_DXX_H
#define CBMFM_LIB_BASE_DXX_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cbmfm_types.h"


/** \brief  Lowest track number for disk images
 */
#define CBMFM_DXX_TRACK_MIN     1

/** \brief  Highest track number for D64 images
 *
 * This is the standard number tracks, SpeedDOS and other modifications can
 * extend this to 40, and the 1541 is capable of reading up to 42 tracks
 */
#define CBMFM_D64_TRACK_MAX     35

/** \brief  Highest track number for D71 images
 */
#define CBMFM_D71_TRACK_MAX     70

/** \brief  Highest track number for D80 images
 */
#define CBMFM_D80_TRACK_MAX     77

/** \brief  Highest track number for D81 images
 */
#define CBMFM_D81_TRACK_MAX     80

/** \brief  Highest track number for D82 images
 */
#define CBMFM_D82_TRACK_MAX     154


/** \brief  Number of blocks in a 35-track d64 image
 */
#define CBMFM_D64_BLOCK_COUNT       683

/** \brief  Number of blocks in a 40-track d64 image
 */
#define CBMFM_D64_BLOCK_COUNT_EXT   768



/** \brief  Size of a standard 35-track image, no error bytes
 */
#define CBMFM_D64_SIZE_STD      174848

/** \brief  Size of a standard 35-track image with error bytes
 */
#define CBMFM_D64_SIZE_STD_ERR  (CBMFM_D64_SIZE_STD + 683)

/** \brief  Size of an extended 40-track image, no error bytes
 */
#define CBMFM_D64_SIZE_EXT      196608

/** \brief  Size of an extended 40-track image with error bytes
 */
#define CBMFM_D64_SIZE_EXT_ERR  (CBMFM_D64_SIZE_EXT + 768)

/** \brief  Minimum size of a d64 image
 *
 * Size of a standard 35-track image without error info
 */
#define CBMFM_D64_SIZE_MIN      CBMFM_D64_SIZE_MIN

/** \brief  Maximum size of a d64 image
 *
 * Size of an extended 40-track image with error info
 */
#define CBMFM_D64_SIZE_MAX      CBMFM_D64_SIZE_EXT_ERR


#define CBMFM_D64_DIR_TRACK     18

/** \brief  BAM track number
 */
#define CBMFM_D64_BAM_TRACK     18

/** \brief  BAM sector number
 */
#define CBMFM_D64_BAM_SECTOR    0


#define CBMFM_D64_BAM_DIR_TRACK     0x00
#define CBMFM_D64_BAM_DIR_SECTOR    0x01
#define CBMFM_D64_BAM_DISK_DOS_VER  0x02

/** \brief  Offset in BAM of track entries
 *
 * Each track entry consists of four bytes, the first is a count of free
 * sectors in the track and the next tree bytes are bitmap of free sectors
 * in the track.
 */
#define CBMFM_D64_BAM_ENTRIES       0x04

#define CBMFM_D64_BAMENT_SIZE       4

/** \brief  Offset in BAM of disk name (16 bytes)
 */
#define CBMFM_D64_BAM_DISK_NAME 0x90

/** \brief  Offset in BAM of disk ID (2 bytes, or 5 bytes)
 */
#define CBMFM_D64_BAM_DISK_ID   0xa2



int         cbmfm_dxx_block_number(const cbmfm_dxx_speedzone_t *zones,
                                   int track, int sector);
intmax_t    cbmfm_dxx_block_offset(const cbmfm_dxx_speedzone_t *zones,
                                   int track, int sector);
bool        cbmfm_dxx_block_read(cbmfm_block_t *block,
                                 cbmfm_dxx_image_t *image,
                                 int track, int sector);

int         cbmfm_dxx_track_block_count(cbmfm_dxx_image_t *image, int track);

#endif
