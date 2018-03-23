/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/cbmfm_types.h
 * \brief   Base library types
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

#ifndef CBMFM_LIB_TYPES_H
#define CBMFM_LIB_TYPES_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/** \brief  Length of a CBMDOS filename
 */
#define CBMFM_CBMDOS_FILENAME_LEN   16


/** \brief  Image flag: read-only bit
 *
 * When set, the image is assumed to be read only and cannot be written back
 * to its original file.
 */
#define CBMFM_IMAGE_FLAGS_READONLY  0x01U


/** \brief  Image flag: dirty bit
 *
 * When set, the image data has been altered since it was opened.
 */
#define CBMFM_IMAGE_FLAGS_DIRTY     0x02U


/** \brief  Image type enumerators
 */
typedef enum {
    CBMFM_IMAGE_TYPE_INVALID = -1,  /**< invalid/uninitialized */
    CBMFM_IMAGE_TYPE_ARK,           /**< ARK archive */
    CBMFM_IMAGE_TYPE_D64,           /**< D64 disk image */
    CBMFM_IMAGE_TYPE_D71,           /**< D71 disk image */
    CBMFM_IMAGE_TYPE_D80,           /**< D80 disk image */
    CBMFM_IMAGE_TYPE_D81,           /**< D81 disk image */
    CBMFM_IMAGE_TYPE_D82,           /**< D82 disk image */
    CBMFM_IMAGE_TYPE_LNX,           /**< Lynx archive */
    CBMFM_IMAGE_TYPE_T64,           /**< T64 archive */

    /** \brief  Number of image types
     */
    CBMFM_IMAGE_TYPE_COUNT
} cbmfm_image_type_t;



/** \brief  track,sector link
 */
typedef struct cbmfm_link_s {
    int track;  /**< track number */
    int sector; /**< sector number */
} cbmfm_link_t;


typedef struct cbmfm_dirent_s {
    uint8_t     filename[CBMFM_CBMDOS_FILENAME_LEN];    /**< CBMDOS filename */
    uint8_t *   filedata;   /**< raw file data (optional) */
    size_t      filesize;   /**< raw file size (optonal) */
    uint8_t     filetype;   /**< CBMDOS file type and flags */

} cbmfm_dirent_t;



typedef struct cbmfm_image_s {
    uint8_t *   data;   /**< raw image data */
    size_t      size;   /**< size of raw image data */
    char *      path;   /**< path to image file */
    cbmfm_image_type_t type;    /**< image type */
    uint32_t    flags;  /**< image flags */
} cbmfm_image_t;





#endif
