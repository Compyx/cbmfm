/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/zipcode.c
 * \brief   Zipcode handling
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
#include "lib/base/errors.h"
#include "lib/base/log.h"

#include "zipcode.h"


/** \brief  Decode RLE data
 *
 * \param[out]  dst destination of decoded data
 * \param[in]   src pointer to the zipcode block to decode (must point at the
 *                  first byte of the block, the track+flags byte)
 *
 * \return  number of bytes used during decoding, including the track+flags and
 *          sector byte in the zipcode block, or -1 on error
 *
 * \throw   #CBMFM_ERR_BUFFER_UNDERFLOW not enough RLE data to fill block
 * \throw   #CBMFM_ERR_BUFFER_OVERFLOW  RLE data would overflow block
 *
 * \ingroup lib_archive_zipcode
 */
static int zipcode_rle_decode(uint8_t *dst, const uint8_t *src)
{
    size_t rle_len = src[CBMFM_ZIPCODE_BLOCK_RLE_LEN];
    uint8_t pack_byte = src[CBMFM_ZIPCODE_BLOCK_RLE_PACK_BYTE];
    const uint8_t *rle_data = src + CBMFM_ZIPCODE_BLOCK_RLE_DATA;
    size_t dst_idx = 0;
    size_t rle_idx = 0;

    cbmfm_log_debug("decoding RLE data: len = %zu, pack-byte = %u\n",
            rle_len, (unsigned int)pack_byte);

    while (dst_idx < CBMFM_BLOCK_SIZE_RAW && rle_idx < rle_len) {

        /* run or normal data? */
        if (rle_data[rle_idx] != pack_byte) {
            /* normal data, copy */
            dst[dst_idx] = rle_data[rle_idx];
            dst_idx++;
            rle_idx++;

        } else {
            /* handle run */
            int len = rle_data[++rle_idx];      /* run length */
            uint8_t b = rle_data[++rle_idx];    /* run value */

            rle_idx++;  /* idx now 'points at' the first byte of RLE data */

            while (len-- > 0 && dst_idx < CBMFM_BLOCK_SIZE_RAW) {
                dst[dst_idx++] = b;
            }
            if (dst_idx == CBMFM_BLOCK_SIZE_RAW && len > 0) {
                /* buffer overflow */
                cbmfm_errno = CBMFM_ERR_BUFFER_OVERFLOW;
                return -1;
            }
        }
    }
    if (dst_idx < CBMFM_BLOCK_SIZE_RAW) {
        /* insufficient data */
        cbmfm_errno = CBMFM_ERR_BUFFER_UNDERFLOW;
        return -1;
    }

    return (int)rle_idx;
}



/** \brief  Unpack zipcoded block \a src into \a dst
 *
 * \param[out]  dst destination of unpacked data
 * \param[in]   src zipcode block entry
 *
 * \return  number of bytes used from \a src, or -1 on error
 *
 * \throw   #CBMFM_ERR_INVALID_DATA     compression flags are invalid (ie 0xc0)
 * \throw   #CBMFM_ERR_BUFFER_UNDERFLOW not enough RLE data to fill block
 * \throw   #CBMFM_ERR_BUFFER_OVERFLOW  RLE data would overflow block
 *
 * \ingroup lib_archive_zipcode
 */
int cbmfm_zipcode_unpack(uint8_t *dst, const uint8_t *src)
{
    switch (src[CBMFM_ZIPCODE_BLOCK_TRACK] & 0xc0) {
        case CBMFM_ZIPCODE_FLAG_STORE:
            /* copy 256 bytes */
            memcpy(dst, src + CBMFM_ZIPCODE_BLOCK_DATA, CBMFM_BLOCK_SIZE_RAW);
            return CBMFM_BLOCK_SIZE_RAW + 2;
        case CBMFM_ZIPCODE_FLAG_FILL:
            /* fill 256 bytes with the same value */
            memset(dst, src[CBMFM_ZIPCODE_BLOCK_FILL_BYTE],
                    CBMFM_BLOCK_SIZE_RAW);
            return CBMFM_BLOCK_SIZE_RAW + 2;
        case CBMFM_ZIPCODE_FLAG_RLE:
            /* TODO: decode RLE sequence */
            return zipcode_rle_decode(dst, src);

        default:
            /* invalid flags value */
            cbmfm_errno = CBMFM_ERR_INVALID_DATA;
            return -1;
    }
}
