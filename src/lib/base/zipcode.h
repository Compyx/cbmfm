/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/zipcode.h
 * \brief   Zipcode handling - header
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


#ifndef CBMFM_LIB_BASE_ZIPCODE_H
#define CBMFM_LIB_BASE_ZIPCODE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "lib/base/errors.h"

/** \brief  Zipcode block encoding flags
 *
 * Bits 7 and 6 of the track byte of a zipcode block entry indicate the storage
 * method used for the block data.
 *
 * \ingroup lib_archive_zipcode
 */
typedef enum cbmfm_zipcode_flag_e {
    CBMFM_ZIPCODE_FLAG_STORE = 0x00, /**< block stored as-is */
    CBMFM_ZIPCODE_FLAG_FILL  = 0x40, /**< block is filled with a single value */
    CBMFM_ZIPCODE_FLAG_RLE   = 0x80  /**< block data has been run-length encoded */
} cbmfm_zipcode_flag_t;

/** \brief  Offset in a zipcode block of the track & flags byte
 *
 * Bits 0-5 indicate the track number, bits 6-7 indicate the compression method
 * use (\see cbmfm_zipcode_flag_t)
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_TRACK           0x00

/** \brief  Offset in a zipcode block of the sector number
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_SECTOR          0x01

/** \brief  Offset in a zipcode block of the data to copy
 *
 * Used in case the block data is simply stored as a 256 byte block.
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_DATA            0x02

/** \brief  Offset in a zipcode block of the fill byte
 *
 * Used in case the block data is a single byte, which gets stored 256 times
 * in the target block.
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_FILL_BYTE       0x02

/** \brief  Offset in a zipcode block of the length of the RLE data
 *
 * Used in case the block data is RLE encoded.
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_RLE_LEN         0x02

/** \brief  Offset in a zipcode block of the 'pack byte'
 *
 * Used in case the block data is RLE encoded. The pack byte indicates a run
 * is coming in the block data.
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_RLE_PACK_BYTE   0x03

/** \brief  Offset in a zipcode block of the RLE data
 *
 * Used in case the block data is RLE encoded.
 *
 * \ingroup lib_archive_zipcode
 */
#define CBMFM_ZIPCODE_BLOCK_RLE_DATA        0x04


int cbmfm_zipcode_unpack(uint8_t *dst, const uint8_t *src);

/** @} */

#endif
