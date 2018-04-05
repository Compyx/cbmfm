/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/d64.c
 * \brief   D64 disk image handling
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

#include "cbmfm_types.h"
#include "base.h"
#include "base/image.h"
#include "base/dxx.h"

#include "d64.h"


/** \brief  Speed zones for D64 images
 */
static const cbmfm_dxx_speedzone_t zones_d64[] = {
    {  1, 17, 21 },
    { 18, 24, 19 },
    { 25, 30, 18 },
    { 31, 40, 17 },
    { -1, -1, -1 }
};


/** \brief  Allocate a d64 image object
 *
 * \return  heap-allocated d64 image object, uninitialized
 */
cbmfm_d64_t *cbmfm_d64_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_d64_t));
}


/** \brief  Initialize \a image to a usable state
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_init(cbmfm_d64_t *image)
{
    cbmfm_image_init((cbmfm_image_t *)image);
    image->type = CBMFM_IMAGE_TYPE_D64;
    image->zones = zones_d64;
}


/** \brief  Allocate and initialize a d64 image
 *
 * \return  new d64 image
 */
cbmfm_d64_t *cbmfm_d64_new(void)
{
    cbmfm_d64_t *image = cbmfm_d64_alloc();
    cbmfm_d64_init(image);
    return image;
}


/** \brief  Free members of \a image, but not \a image itself
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_cleanup(cbmfm_d64_t *image)
{
    cbmfm_image_cleanup((cbmfm_image_t *)image);
}


/** \brief  Free members of \a image and \a image itself
 *
 * \param[in,out]   image   d64 image
 */
void cbmfm_d64_free(cbmfm_d64_t *image)
{
    cbmfm_d64_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read d64 file \a name into \a image
 *
 * \param[in,out]   image   d64 image
 * \param[in]       name    image file name
 *
 * \return  true on success
 *
 * \throw   #CBMFM_ERR_IO
 * \throw   #CBMFM_ERR_SIZE_MISMATCH
 */
bool cbmfm_d64_open(cbmfm_d64_t *image, const char *name)
{
    if (!cbmfm_image_read_data((cbmfm_image_t *)image, name)) {
        return false;
    }

    /* check size */
    if (image->size < CBMFM_D64_SIZE_MIN || image->size > CBMFM_D64_SIZE_MAX) {
        cbmfm_free(image->data);
        image->data = NULL;
        cbmfm_errno = CBMFM_ERR_SIZE_MISMATCH;
        return false;
    }
    return true;
}

