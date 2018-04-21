/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/lnx.h
 * \brief   Lynx archive handling - header
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

#ifndef CBMFM_LIB_IMAGE_LNX_H
#define CBMFM_LIB_IMAGE_LNX_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cbmfm_types.h"


/** \brief  Minimum size for an empty Lynx container
 *
 * Since Lynx containers are aligned at blocks, the minimum size is
 * 254 + 2 bytes (254 bytes for the block data, 2 bytes for the load
 * address).
 */
#define CBMFM_LNX_MIN_SIZE  (CBMFM_BLOCK_SIZE_DATA + 2)


/** \brief  Load address of Lynx files
 */
#define CBMFM_LNX_LOAD_ADDR 0x0801


/** \brief  BASIC 'magic bytes' offset
 */
#define CBMFM_LNX_BASIC_OFFSET  0x02

/** \brief  BASIC 'magic bytes' max size
 */
#define CBMFM_LNX_BASIC_SIZE     0x5c


#define CBMFM_LNX_HDR           0x5e

#define CBMFM_LNX_VERSION_LEN   0x18


#define CBBMFM_LNX_DIRENT_FILENAME  0x00



cbmfm_lnx_t *   cbmfm_lnx_alloc(void);
void            cbmfm_lnx_init(cbmfm_lnx_t *image);
cbmfm_lnx_t *   cbmfm_lnx_new(void);
void            cbmfm_lnx_cleanup(cbmfm_lnx_t *image);
void            cbmfm_lnx_free(cbmfm_lnx_t *image);


bool            cbmfm_lnx_open(cbmfm_lnx_t *image, const char *path);
void            cbmfm_lnx_dump(const cbmfm_lnx_t *image);
cbmfm_dir_t *   cbmfm_lnx_dir_read(cbmfm_lnx_t *image);

bool            cbmfm_lnx_file_read(cbmfm_dir_t *dir,
                                    cbmfm_file_t *file,
                                    uint16_t index);
#endif
