/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/t64.c
 * \brief   T64 handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
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
#include <string.h>

#include "cbmfm_types.h"
#include "base.h"
#include "base/image.h"
#include "base/dir.h"

#include "t64.h"

/** \ingroup    lib_archive_t64
 * @{
 */


static void cbmfm_t64_parse_header(cbmfm_t64_t *image)
{
    uint8_t *data = image->data;
    int i;

    /* tape 'magic' */
    memcpy(image->magic, data + CBMFM_T64_HDR_MAGIC, CBMFM_T64_HDR_MAGIC_LEN);
    /* terminate */
    image->magic[CBMFM_T64_HDR_MAGIC_LEN - 1] = '\0';

    /* tape name */
    memcpy(image->tape_name, data + CBMFM_T64_HDR_TAPE_NAME,
            CBMFM_T64_HDR_TAPE_NAME_LEN);
    /* remove trailing spaces */
    i = CBMFM_T64_HDR_TAPE_NAME_LEN - 1;
    while (i >= 0 && image->tape_name[i] == 0x20) {
        image->tape_name[i] = '\0';
        i--;
    }

    /* version */
    image->version = cbmfm_word_get_le(data + CBMFM_T64_HDR_VERSION);

    /* max entries in dir */
    image->entry_max = cbmfm_word_get_le(data + CBMFM_T64_HDR_DIR_MAX);
    /* used entries in dir */
    image->entry_used = cbmfm_word_get_le(data + CBMFM_T64_HDR_DIR_USED);

}

/** \brief  Allocate a t64 image
 *
 * \return  new t64 image
 */
cbmfm_t64_t *cbmfm_t64_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_t64_t));
}


/** \brief  Initialize t64 image
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_init(cbmfm_t64_t *image)
{
    cbmfm_image_init((cbmfm_image_t *)image);
    image->type = CBMFM_IMAGE_TYPE_T64;

    memset(image->magic, 0x00, CBMFM_T64_HDR_MAGIC_LEN);
    memset(image->tape_name, 0x20, CBMFM_T64_HDR_TAPE_NAME_LEN);
    image->tape_name[CBMFM_T64_HDR_TAPE_NAME_LEN] = '\0';
    image->version = 0;
    image->entry_max = 0;
    image->entry_used = 0;
}


/** \brief  Allocate an initialize a t64 image
 *
 * \return  new t64 image
 */
cbmfm_t64_t *cbmfm_t64_new(void)
{
    cbmfm_t64_t *image = cbmfm_t64_alloc();
    cbmfm_t64_init(image);
    return image;
}


/** \brief  Free members of \a image but not \a image itself
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_cleanup(cbmfm_t64_t *image)
{
    cbmfm_image_cleanup((cbmfm_image_t *)image);
}


/** \brief  Free members of \a image and \a image itself
 *
 * \param[in,out]   image   t64 image
 */
void cbmfm_t64_free(cbmfm_t64_t *image)
{
    cbmfm_t64_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read data from \a path into \a image
 *
 * \param[in,out]   image   t64 image
 * \param[in]       path    path to image data
 *
 * \return  bool
 */
bool cbmfm_t64_open(cbmfm_t64_t *image, const char *path)
{
    if (!cbmfm_image_read_data((cbmfm_image_t *)image, path)) {
        return false;
    }
    cbmfm_t64_parse_header(image);
    return true;
}


/** \brief  Dump T64 header data on stdout
 *
 * \param[in]   image   t64 image
 */
void cbmfm_t64_dump_header(const cbmfm_t64_t *image)
{
    printf("tape magic    : '%s'\n", image->magic);
    printf("tape name     : '%s'\n", image->tape_name);
    printf("tape version  : $%04x\n", image->version);
    printf("dir entry max : %d\n", (int)(image->entry_max));
    printf("dir entry used: %d\n", (int)(image->entry_used));
}


/** @} */
