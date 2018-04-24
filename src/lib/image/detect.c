/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/image/detect.c
 * \brief   Image auto-detect functions
 *
 * This module attempts to detect the image type of a give file. First image
 * types with a recognizable signature are checked, then images types with
 * fixed sizes and finally as a fall back, the file extension is used to
 * determine the image type.
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
#include <inttypes.h>

#include "lib/base/log.h"
#include "lib/image/ark.h"
#include "lib/image/d64.h"
#include "lib/image/lnx.h"
#include "lib/image/t64.h"

#include "lib/image/detect.h"


/** \brief  Try to detect the image type of \a filename
 *
 * \param[in]   filename    filename/path
 *
 * \return  image type or #CBMFM_IMAGE_TYPE_INVALID
 */
int cbmfm_image_detect_type(const char *filename)
{

    cbmfm_log_debug("trying to detect image type of '%s'\n", filename);

    /* first check files with header 'magic' */
    if (cbmfm_is_lnx(filename)) {
        return CBMFM_IMAGE_TYPE_LNX;
    }
    if (cbmfm_is_t64(filename)) {
        return CBMFM_IMAGE_TYPE_T64;
    }

    /* check files with fixed sizes */
    if (cbmfm_is_d64(filename)) {
        return CBMFM_IMAGE_TYPE_D64;
    }

    /* check file extensions */
    if (cbmfm_is_ark(filename)) {
        return CBMFM_IMAGE_TYPE_ARK;
    }

    /* give up */
    return CBMFM_IMAGE_TYPE_INVALID;
}
