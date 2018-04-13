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


/** \ingroup    lib_image
 * @{
 */

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


/** \brief  D64 directory track number
 *
 * This is a fixed number in the drive ROM, although the BAM seems to allow
 * setting the dir track number.
 */
#define CBMFM_D64_DIR_TRACK     18

/** \brief  D64 directory sector number
 *
 * This is a fixed number in the drive ROM, although the BAM seems to allow
 * setting the dir sector number.
*/
#define CBMFM_D64_DIR_SECTOR    1


/** \brief  BAM track number
 */
#define CBMFM_D64_BAM_TRACK     18

/** \brief  BAM sector number
 */
#define CBMFM_D64_BAM_SECTOR    0


/** \brief  Offset in BAM of directory track number
 *
 * This is completely ignored by all drive ROMS I'm aware of.
 */
#define CBMFM_D64_BAM_DIR_TRACK     0x00

/** \brief  Offset in BAM of directory sector number
 *
 * This is completely ignored by all drive ROMS I'm aware of.
 */
#define CBMFM_D64_BAM_DIR_SECTOR    0x01

/** \brief  Offset in BAM of disk DOS version byte
 */
#define CBMFM_D64_BAM_DISK_DOS_VER  0x02

/** \brief  Offset in BAM of track entries
 *
 * Each track entry consists of four bytes, the first is a count of free
 * sectors in the track and the next tree bytes are bitmap of free sectors
 * in the track.
 */
#define CBMFM_D64_BAM_ENTRIES       0x04

/** \brief  Size of a BAM entry for a single track
 *
 * A 1541 BAM entry is four bytes: the first byte keeps a count of the number
 * of free blocks in the track, and the next three bytes present a bitmap with
 * each bit indicating if a particular block is free in the sector.
 */
#define CBMFM_D64_BAMENT_SIZE       4

/** \brief  Offset in BAM of disk name (16 bytes)
 */
#define CBMFM_D64_BAM_DISK_NAME 0x90

/** \brief  Offset in BAM of disk ID (2 bytes, or 5 bytes)
 */
#define CBMFM_D64_BAM_DISK_ID   0xa2


/*
 * D64 directory entry constants
 */


/** \brief  Offset of the next directory block track number
 *
 * This is only valid for the first directory entry at 0x00 and is just the
 * track part of the (track,sector) link of the direcctory block.
 */
#define CBMFM_D64_DIRENT_NEXT_DIR_TRACK     0x00

/** \brief  Offset of the next directory block sector number
 *
 * This is only valid for the first directory entry at 0x00 and is just the
 * track part of the (track,sector) link of the direcctory block.
 */
#define CBMFM_D64_DIRENT_NEXT_DIR_SECTOR    0x01

/** \brief  Offset of the CBMDOS filetype and flags
 */
#define CBMFM_D64_DIRENT_FILE_TYPE          0x02

/** \brief  Offset of the file's first block track number
 */
#define CBMFM_D64_DIRENT_FILE_TRACK         0x03

/** \brief  Offset of the file's first block sector number
 */
#define CBMFM_D64_DIRENT_FILE_SECTOR        0x04

/** \brief  Offset of the file name in PETSCII (16 bytes)
 */
#define CBMFM_D64_DIRENT_FILE_NAME          0x05

/** \brief  Offset of the REL file side sector block track number
 */
#define CBMFM_D64_DIRENT_REL_SSB_TRACK      0x15

/** \brief  Offset of the REL file side sector block sector number
 */
#define CBMFM_D64_DIRENT_REL_SSB_SECTOR     0x16

/** \brief  Offset of the REL file record size
 */
#define CBMFM_D64_DIRENT_REL_REC_SIZE       0x17

/* TODO: GEOS uses 0x18-0x1d */

/** \brief  Offset of the file's size in blocks (LSB)
 */
#define CBMFM_D64_DIRENT_BLOCKS_LSB         0x1e

/** \brief  Offset of the file's size in blocks (MSB)
 */
#define CBMFM_D64_DIRENT_BLOCKS_MSB         0x1f


/** \brief  Size of a raw directory entry of Dxx images
 */
#define CBMFM_DXX_DIRENT_SIZE               0x20



int         cbmfm_dxx_block_number(const cbmfm_dxx_speedzone_t *zones,
                                   int track, int sector);
intmax_t    cbmfm_dxx_block_offset(const cbmfm_dxx_speedzone_t *zones,
                                   int track, int sector);
bool        cbmfm_dxx_block_read(cbmfm_block_t *block,
                                 cbmfm_dxx_image_t *image,
                                 int track, int sector);

int         cbmfm_dxx_track_block_count(cbmfm_dxx_image_t *image, int track);

bool        cbmfm_dxx_block_iter_init(cbmfm_dxx_block_iter_t *iter,
                                      cbmfm_dxx_image_t *image,
                                      int track, int sector);
bool        cbmfm_dxx_block_iter_next(cbmfm_dxx_block_iter_t *iter);
void        cbmfm_dxx_block_iter_read_data(cbmfm_dxx_block_iter_t *iter,
                                           uint8_t *dest);

void        cbmfm_dxx_dirent_init(cbmfm_dirent_t *dirent, int type);

/** @} */

#endif
