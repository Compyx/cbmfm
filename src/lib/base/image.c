/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/image.c
 * \brief   Base image functions
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
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "cbmfm_types.h"
#include "base/mem.h"
#include "base/io.h"

#include "base/image.h"

/**
 * \ingroup lib_base
 * @{
 */


/** \brief  Image type meta data object
 */
typedef struct image_type_meta_s {
    const char *ext;    /**< file extension */
    const char *desc;   /**< image description */
} image_type_meta_t;


/** \brief  Table with some meta data on image types
 *
 * This data is mostly used for display purposes.
 */
static image_type_meta_t image_meta_data[CBMFM_IMAGE_TYPE_COUNT] = {
    { "ark",    "ARK archive" },
    { "d64",    "D64 35-track disk image" },
    { "d71",    "D71 70-track disk image" },
    { "d80",    "D80 77-track disk image" },
    { "d81",    "D81 80-track disk image" },
    { "d82",    "D82 154-track disk image" },
    { "lnx",    "Lynx archive" },
    { "t64",    "T64 archive" }
};


/** \brief  Initialize \a block
 *
 * Sets track and sector numbers to -1, marking it invalid, sets pointers to
 * `NULL`.
 *
 * \param[in,out]   block   block object
 */
void cbmfm_block_init(cbmfm_block_t *block)
{
    block->track = -1;
    block->sector = -1;
    block->data = NULL;
    block->next = NULL;
    block->prev = NULL;
}


/** \brief  Free block data inside \a block
 *
 * Frees the 256 bytes inside \a block
 *
 * \param[in,out]   block   block object
 */
void cbmfm_block_cleanup(cbmfm_block_t *block)
{
    cbmfm_free(block->data);
    block->data = NULL;
}


/** \brief  Dump block data on stdout
 *
 * \param[in]   block   block object
 */
void cbmfm_block_dump(const cbmfm_block_t *block)
{
    int row;
    int col;

    for (row = 0; row < 16; row++) {
        printf("%02d:%02d:%02x: ", block->track, block->sector, row * 16);
        for (col = 0; col < 16; col++) {
            printf("%02x ", block->data[row * 16 + col]);
        }
        for (col = 0; col < 16; col++) {
            int ch = block->data[row * 16 + col];
            putchar(isprint(ch) ? ch : '.');
        }
        putchar('\n');
    }
}


/** \brief  Get image file extension
 *
 * \param[in]   type    image type
 *
 * \return  file extension, excluding dot
 */
const char *cbmfm_image_type_get_ext(cbmfm_image_type_t type)
{
    if (type < 0 || type >= CBMFM_IMAGE_TYPE_COUNT) {
        return "???";
    }
    return image_meta_data[type].ext;
}


/** \brief  Get image file description
 *
 * \param[in]   type    image type
 *
 * \return  description
 */
const char *cbmfm_image_type_get_desc(cbmfm_image_type_t type)
{
    if (type < 0 || type >= CBMFM_IMAGE_TYPE_COUNT) {
        return "n/a";
    }
    return image_meta_data[type].desc;
}


/** \brief  Initialize \a image to a usable state
 *
 * Initializes all fields to 0/NULL and sets image type to invalid
 *
 * \param[in,out]   image   image handle
 */
void cbmfm_image_init(cbmfm_image_t *image)
{
    image->data = NULL;
    image->size = 0;
    image->path = NULL;
    image->flags = 0;
    image->type = CBMFM_IMAGE_TYPE_INVALID;
}


/** \brief  Allocate memory for image handle and init members
 *
 * Allocates an image handle and initializes it.
 *
 * \return  heap-allocated image handle
 */
cbmfm_image_t *cbmfm_image_alloc(void)
{
    cbmfm_image_t *image = cbmfm_malloc(sizeof *image);
    cbmfm_image_init(image);
    return image;
}


/** \brief  Free \a image members, but not \a image itself
 *
 * Calls cbmfm_init() on \a image afterwards to prepare \a image for optional
 * reuse.
 *
 * \param[in,out]   image   image handle
 */
void cbmfm_image_cleanup(cbmfm_image_t *image)
{
    if (image->path != NULL) {
        cbmfm_free(image->path);
    }
    if (image->data != NULL) {
        cbmfm_free(image->data);
    }
    cbmfm_image_init(image);
}


/** \brief  Free \a image members and \a image itself
 *
 * \param[in,out]   image   image handle
 */
void cbmfm_image_free(cbmfm_image_t *image)
{
    cbmfm_image_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read data from \a path into \a image
 *
 * Reads data from \a path into \a image, heap-allocating the data. This
 * requires a valid image handle initialized with cbmfm_init(). Also stores
 * a copy of \a path in \a image.
 *
 * \param[in,out]   image   image handle
 * \param[in]       path    path to image file
 *
 * \return  bool
 */
bool cbmfm_image_read_data(cbmfm_image_t *image, const char *path)
{
    intmax_t size;

    /* read data, start with a buffer of 1MB */
    size = cbmfm_read_file_sizereq(&(image->data), path, (1U << 20));
    if (size < 0) {
        /* error already set */
        return false;
    }
    image->size = (size_t)size;
    image->path = cbmfm_strdup(path);
    cbmfm_image_set_dirty(image, false);
    return true;
}


/*
 * Image flag handling
 */


/** \brief  Get image flag
 *
 * \param[in]   image   image
 * \param[in]   flag    flag mask
 *
 * \see cbmfm_types.h for the flag constants
 *
 * \return  bool
 */
bool cbmfm_image_get_flag(const cbmfm_image_t *image, uint32_t flag)
{
    return (bool)(image->flags & flag);
}


/** \brief  Set image flag
 *
 * \param[in]   image   image
 * \param[in]   flag    flag mask
 * \param[in]   state   flag state
 *
 * \see cbmfm_types.h for the flag constants
 */
void cbmfm_image_set_flag(cbmfm_image_t *image, uint32_t flag, bool state)
{
    image->flags &= ~flag;
    if (state) {
        image->flags |= flag;
    }
}


/** \brief  Check if \a image is marked read only
 *
 * \param[in]   image   image handle
 *
 * \return  bool
 */
bool cbmfm_image_get_readonly(const cbmfm_image_t *image)
{
    return cbmfm_image_get_flag(image, CBMFM_IMAGE_FLAG_READONLY);
}


/** \brief  Set \a readonly flag on \a image
 *
 * \param[in,out]   image       image handle
 * \param[in]       readonly    read only state
 */
void cbmfm_image_set_readonly(cbmfm_image_t *image, bool readonly)
{
    cbmfm_image_set_flag(image, CBMFM_IMAGE_FLAG_READONLY, readonly);
}


/** \brief  Check if \a image is dirty
 *
 * In this context, dirty means the data in the image has changed since it
 * was opened or written back.
 *
 * \param[in]   image   image handle
 *
 * \return  bool
 */
bool cbmfm_image_get_dirty(const cbmfm_image_t *image)
{
    return cbmfm_image_get_flag(image, CBMFM_IMAGE_FLAG_DIRTY);
}


/** \brief  Set dirty flag on \a image
 *
 * \param[in,out]   image   image handle
 * \param[in]       dirty   image has unsaved changes
 */
void cbmfm_image_set_dirty(cbmfm_image_t *image, bool dirty)
{
    cbmfm_image_set_flag(image, CBMFM_IMAGE_FLAG_DIRTY, dirty);
}


/** \brief  Get invalid flag of \a image
 *
 * \param[in,out]   image   image handle
 *
 * \return  invalid state
 */
bool cbmfm_image_get_invalid(const cbmfm_image_t *image)
{
   return cbmfm_image_get_flag(image, CBMFM_IMAGE_FLAG_INVALID);
}


/** \brief  Set invalid flag on \a image
 *
 * \param[in,out]   image   image handle
 * \param[in]       invalid some operation failed, leaving the image in an
 *                          invalid state
 */
void cbmfm_image_set_invalid(cbmfm_image_t *image, bool invalid)
{
    image->flags &= CBMFM_IMAGE_FLAG_INVALID;
    if (invalid) {
        image->flags |= CBMFM_IMAGE_FLAG_INVALID;
    }
}

/** @} */
