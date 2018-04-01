/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/ark.c
 * \brief   Ark handling
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

#include "ark.h"


/** \brief  Determine offset in ARK data of the first file's data
 *
 * \param[in]   image   image handle
 *
 * \return  offset to data of first file
 */
static size_t ark_file_data_offset(const cbmfm_image_t *image)
{
    size_t required;
    size_t sectors;

    /*
     * minimum size required for the directory:
     * number of entries * sizeof(dirent) + 1 (for the dirent count byte)
     */
    required = image->data[CBMFM_ARK_DIRENT_COUNT]
        * (size_t)(CBMFM_ARK_DIRENT_SIZE + 1);

    /*
     * number of sectors used for the directory:
     * adjust to nearest sector-size boundary (ARK uses 254 byte 'sectors')
     */
    sectors = required / CBMFM_SECTOR_SIZE_DATA;
    if (required % CBMFM_SECTOR_SIZE_DATA != 0) {
        sectors++;
    }

    return sectors * CBMFM_SECTOR_SIZE_DATA;
}


/** \brief  Open ark archive \a path
 *
 * \param[in,out]   image   image handle
 * \param[in]       path    path to ARK file
 *
 * \return  bool
 */
bool cbmfm_ark_open(cbmfm_image_t *image, const char *path)
{
    cbmfm_image_init(image);
    image->type = CBMFM_IMAGE_TYPE_ARK;
    cbmfm_image_set_readonly(image, true);
    return cbmfm_image_read_data(image, path);
}


/** \brief  Free memory used by the members of \a image, but not \a image itself
 *
 * \param[in,out]   image   image handle
 */
void cbmfm_ark_cleanup(cbmfm_image_t *image)
{
    cbmfm_image_cleanup(image);
}



void cbmfm_ark_dump_stats(const cbmfm_image_t *image)
{
    printf("filename        : '%s'\n", image->path);
    printf("filesize        : $%04lx\n", (unsigned long)(image->size));
    printf("dir entries     : %u\n", image->data[0]);    /* FIXME */
    printf("file data offset: $%04lx\n", (unsigned long)ark_file_data_offset(image));
}
