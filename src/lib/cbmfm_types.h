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


/** \brief  Size of a raw sector/block
 *
 * Size of a sector/block, including the (track,sector) pointer
 */
#define CBMFM_BLOCK_SIZE_RAW       0x100

/** \brief  Size of the data section of a sector
 *
 * Size of a sector/block without the (track,sector) pointer
 */
#define CBMFM_BLOCK_SIZE_DATA      0xfe


/** \brief  Length of a CBMDOS filename
 */
#define CBMFM_CBMDOS_FILE_NAME_LEN  0x10


/** \brief  Length of a CBMDOS disk name
 */
#define CBMFM_CBMDOS_DISK_NAME_LEN  0x10

/** \brief  Length of a standard disk ID
 */
#define CBMFM_CBMDOS_DISK_ID_LEN        0x02

/** \brief  Length of an extended disk ID
 *
 * Many scene releases use the unused byte after the ID and the 2-byte
 * 'DOS type' to get a 5-byte ID.
 */
#define CBMFM_CBMDOS_DISK_ID_LEN_EXT    0x05


/** \brief  Bitmask for the "closed" bit in a CBMDOS filetype byte
 */
#define CBMFM_CBMDOS_FILE_CLOSED_BIT    0x80U

/** \brief  Bitmask for the "locked" bit in a CBMDOS filetype byte
 */
#define CBMFM_CBMDOS_FILE_LOCKED_BIT    0x40U


/** \brief  Image flag: read-only bit
 *
 * When set, the image is assumed to be read only and cannot be written back
 * to its original file.
 */
#define CBMFM_IMAGE_FLAG_READONLY   0x01U


/** \brief  Image flag: dirty bit
 *
 * When set, the image data has been altered since it was opened.
 */
#define CBMFM_IMAGE_FLAG_DIRTY      0x02U


/** \brief  Image flag: invalid bit
 *
 * When set, this bit indicates an operation on the image altered some data but
 * didn't complete succesfully, leaving the image in an invalid state.
 */
#define CBMFM_IMAGE_FLAG_INVALID    0x04U


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



/** \brief  Block object
 *
 * Represents a single block on disk. Contains a track and sector number, with
 * optional fields for block data and linked list pointers for iterating a
 * chain of blocks.
 */
typedef struct cbmfm_block_s {
    int         track;          /**< track number */
    int         sector;         /**< sector number */
    uint8_t *   data;           /**< block data (optional) */
    struct cbmfm_block_s *next; /**< next block (optional) */
    struct cbmfm_block_s *prev; /**< previous block (optional) */
} cbmfm_block_t;


/** \brief  T64 specific dirent fields
 */
typedef struct cbmfm_dirent_t64_s {
    uint32_t    data_offset;
    uint16_t    load_addr;
    uint16_t    end_addr;
    uint8_t     c64s_type;
    uint16_t    dir_index;
} cbmfm_dirent_t64_t;


/** \brief  Directory entry object
 *
 * Contains information on a directory entry.
 */
typedef struct cbmfm_dirent_s {
    uint8_t     filename[CBMFM_CBMDOS_FILE_NAME_LEN];    /**< CBMDOS filename */
    uint8_t *   filedata;   /**< raw file data (optional) */
    size_t      filesize;   /**< raw file size (optional) */
    uint8_t     filetype;   /**< CBMDOS file type and flags */

    cbmfm_block_t first_block;      /**< first block */
    uint16_t    size_blocks;        /**< file size in blocks */

    struct cbmfm_dir_s *dir;        /**< parent directory reference */
    struct cbmfm_image_s *image;    /**< parent image reference */

    /** \brief  Extra type-specific data
     */
    union {
        cbmfm_dirent_t64_t t64;     /**< T64 specific data */
    } extra;

} cbmfm_dirent_t;


/** \brief  Directory object
 *
 */
typedef struct cbmfm_dir_s {
    cbmfm_dirent_t **entries;       /**< array of dirent pointers */
    size_t entry_max;               /**< size of entries array */
    size_t entry_used;              /**< number of used entries */

    struct cbmfm_image_s *image;    /**< parent image reference */

} cbmfm_dir_t;



#define CBMFM_IMAGE_SHARED_MEMBERS \
    uint8_t *   data;   /**< raw image data */ \
    size_t      size;   /**< size of raw image data */ \
    char *      path;   /**< path to image file */ \
    cbmfm_image_type_t type;    /**< image type */ \
    uint32_t    flags;  /**< image flags */


/** \brief  Image object
 */
typedef struct cbmfm_image_s {
    CBMFM_IMAGE_SHARED_MEMBERS
} cbmfm_image_t;


/** \brief  Speed zone entry
 */
typedef struct cbmfm_dxx_speedzone_s {
    int trk_lo; /**< lowest track number for zone */
    int trk_hi; /**< highest track number for zone */
    int blocks; /**< number of blocks/sectors per track for zone */
} cbmfm_dxx_speedzone_t;


/** \brief  Dxx image shared members
 */
#define CBMFM_DXX_IMAGE_SHARED_MEMBERS \
    const cbmfm_dxx_speedzone_t *zones; /**< speed zones */ \
    int track_max;  /**< maximum track number */ \
    bool errors;    /**< image contains error bytes */


/** \brief  Dxx image
 */
typedef struct cbmfm_dxx_image_s {
    CBMFM_IMAGE_SHARED_MEMBERS
    CBMFM_DXX_IMAGE_SHARED_MEMBERS
} cbmfm_dxx_image_t;


/** \brief  D64 image
 */
typedef struct cbmfm_d64_s {
    CBMFM_IMAGE_SHARED_MEMBERS
    CBMFM_DXX_IMAGE_SHARED_MEMBERS
} cbmfm_d64_t;


/** \brief  Disk image block iterator
 *
 * An object to iterate over a block chain of a file/directory listing.
 * The `curr` member contains information on the current block the iterator is
 * at, while the `prev` member contains information on the previous block the
 * iterator was at (allows for back-patching track/sector pointers when writing
 * a block chain).
 */
typedef struct cbmfm_dxx_block_iter_s {
    cbmfm_block_t       curr;   /**< current block */
    cbmfm_block_t       prev;   /**< previous block */
    cbmfm_dxx_image_t * image;  /**< parent image reference */
} cbmfm_dxx_block_iter_t;


/** \brief  Directory entry iterator
 */
typedef struct cbmfm_dxx_dir_iter_s {
    cbmfm_dxx_block_iter_t  block_iter;     /**< block iterator */
    size_t                  entry_offset;   /**< offset in block of dirent */
    cbmfm_dxx_image_t *     image;          /**< image reference */
} cbmfm_dxx_dir_iter_t;


/** \brief  Length of the header magic bytes
 */
#define CBMFM_T64_HDR_MAGIC_LEN 0x20

/** \brief  Length of 'tape' name
 */
#define CBMFM_T64_HDR_TAPE_NAME_LEN 0x18


/** \brief  T64 image
 */
typedef struct cbmfm_t64_s {
    CBMFM_IMAGE_SHARED_MEMBERS

    char magic[CBMFM_T64_HDR_MAGIC_LEN];
    char tape_name[CBMFM_T64_HDR_TAPE_NAME_LEN + 1];
    uint16_t version;
    uint16_t entry_max;
    uint16_t entry_used;

} cbmfm_t64_t;



#endif
