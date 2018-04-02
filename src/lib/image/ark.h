/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/ark.h
 * \brief   Ark handling - header
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

#ifndef CBMFM_LIB_IMAGE_ARK_H
#define CBMFM_LIB_IMAGE_ARK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#define CBMFM_ARK_DIRENT_COUNT          0x00
#define CBMFM_ARK_DIR_OFFSET            0x01


/** \brief  Size of an ARK dirent
 */
#define CBMFM_ARK_DIRENT_SIZE           0x1d


/** \brief  CBMDOS filetype and flags (BYTE)
 */
#define CBMFM_ARK_DIRENT_FILETYPE       0x00

/** \brief  Number of bytes used - 1 in the final sector (BYTE)
 */
#define CBMFM_ARK_DIRENT_LAST_SEC_USED  0x01

/** \brief  CBMDOS filename (16 bytes)
 */
#define CBMFM_ARK_DIRENT_FILENAME       0x02

/** \brief  File size in blocks (WORD)
 */
#define CBMFM_ARK_DIRENT_FILESIZE       0x1b


/*
 * Note: RELative files are unsupported, the constants are added for
 * completeness
 */

/** \brief  REL file record size (BYTE)
 */
#define CBMFM_ARK_DIRENT_REL_RECSIZE    0x12

/** \brief  REL file side-sector-block count (BYTE)
 */
#define CBMFM_ARK_DIRENT_SSB_COUNT      0x19

/** \brief  REL file bytes used for last side sector (BYTE)
 */
#define CBMFM_ARK_DIRENT_LAST_SS_USED   0x1a



bool cbmfm_ark_open(cbmfm_image_t *image, const char *path);
void cbmfm_ark_cleanup(cbmfm_image_t *image);

void cbmfm_ark_dump_stats(const cbmfm_image_t *image);


cbmfm_dir_t *cbmfm_ark_read_dir(cbmfm_image_t *image, bool read_file_data);

#endif
