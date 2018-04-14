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


/** \brief  Header size
 */
#define CBMFM_T64_HDR_SIZE      0x40

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


/** \brief  Offset in a T64 of the directory
 */
#define CBMFM_T64_DIR_OFFSET    CBMFM_T64_HDR_SIZE


/** \brief  Size of a T64 directory entry
 */
#define CBMFM_T64_DIRENT_SIZE   0x20


/** \brief  C64s file type
 *
 */
#define CBMFM_T64_DIRENT_C64S_TYPE  0x00

/** \brief  CBMDOS file type
 *
 * Highly unreliable, just assume PRG when the C64s isn't a FRZ file.
 */
#define CBMFM_T64_DIRENT_FILE_TYPE  0x01

/** \brief  Load address (WORD)
 */
#define CBMFM_T64_DIRENT_LOAD_ADDR  0x02

/** \brief  End address (WORD)
 */
#define CBMFM_T64_DIRENT_END_ADDR   0x04

/** \brief  File data offset (DWORD)
 */
#define CBMFM_T64_DIRENT_DATA_OFFSET    0x08

/** \brief  File name (PETSCII, padding with $20)
 */
#define CBMFM_T64_DIRENT_FILE_NAME  0x10



cbmfm_t64_t *   cbmfm_t64_alloc(void);
void            cbmfm_t64_init(cbmfm_t64_t *image);
cbmfm_t64_t *   cbmfm_t64_new(void);
void            cbmfm_t64_cleanup(cbmfm_t64_t *image);
void            cbmfm_t64_free(cbmfm_t64_t *image);
bool            cbmfm_t64_open(cbmfm_t64_t *image, const char *path);


void            cbmfm_t64_dump_header(const cbmfm_t64_t *image);

void            cbmfm_t64_dirent_init(cbmfm_dirent_t *dirent);
bool            cbmfm_t64_dirent_parse(cbmfm_t64_t *image,
                                       cbmfm_dirent_t *dirent,
                                       uint16_t index);

cbmfm_dir_t *   cbmfm_t64_read_dir(cbmfm_t64_t *image);
int             cbmfm_t64_fix_dir(cbmfm_dir_t *dir);


#endif
