/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/t64.h
 * \brief   T64 handling - header
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

#ifndef CBMFM_LIB_IMAGE_T64_H
#define CBMFM_LIB_IMAGE_T64_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cbmfm_types.h"


/** \brief  Header magic bytes (32 bytes, 0-padded)
 *
 * This field seems to vary a lot, depending on which tool was used to create
 * the T64 file.
 */
#define CBMFM_T64_HDR_MAGIC     0x00



/** \brief  T64 version number (WORD)
 *
 * Completely useless, no idea what the different numbers mean.
 */
#define CBMFM_T64_HDR_VERSION   0x20

/** \brief  Maximum number of entries in the 'directory' (WORD)
 */
#define CBMFM_T64_HDR_DIR_MAX   0x22

/** \brief  Number of used entries in the 'directory' (WORD)
 *
 * Highly unreliable, this is 0 many times for T64's with a single entry.
 */
#define CBMFM_T64_HDR_DIR_USED  0x24

/** \brief  'Tape' name (24 bytes of ASCII, padded with $20)
 */
#define CBMFM_T64_HDR_TAPE_NAME 0x28




cbmfm_t64_t *   cbmfm_t64_alloc(void);
void            cbmfm_t64_init(cbmfm_t64_t *image);
cbmfm_t64_t *   cbmfm_t64_new(void);
void            cbmfm_t64_cleanup(cbmfm_t64_t *image);
void            cbmfm_t64_free(cbmfm_t64_t *image);
bool            cbmfm_t64_open(cbmfm_t64_t *image, const char *path);


void cbmfm_t64_dump_header(const cbmfm_t64_t *image);

#endif
