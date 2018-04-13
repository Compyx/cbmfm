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
#include "log.h"

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

    if (image->entry_used == 0) {
        cbmfm_log_warning("%s(): found entry count 0, adjusting to 1\n",
                __func__);
        image->entry_used++;
    }

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


#if 0
/** \brief  Determine offset in \a image of the file data segment
 *
 * \param[in]   image   t64 image
 *
 * \return  offset to file data segment
 */
static size_t t64_data_offset(cbmfm_t64_t *image)
{
    return (size_t)(CBMFM_T64_HDR_SIZE +
            image->entry_max * CBMFM_T64_DIRENT_SIZE);
}


/** \brief  Get pointer to file data segment in \a image
 *
 * \param[in]   image   t64 image
 *
 * \return  pointer to file data segment
 */
static uint8_t *t64_data_ptr(cbmfm_t64_t *image)
{
    return image->data + t64_data_offset(image);
}
#endif


/** \brief  Initialize \a dirent, including t64 specific data
 *
 * \param[out]  dirent
 */
void cbmfm_t64_dirent_init(cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_t64_t *extra = &(dirent->extra.t64);

    cbmfm_dirent_init(dirent);
    extra->data_offset = 0;
    extra->load_addr = 0;
    extra->end_addr = 0;
    extra->c64s_type = 0;
    extra->dir_index = 0;
}


/** \brief  Parse directory entry at \a index
 *
 * \param[in]   image   t64 image
 * \param[out]  dirent  directory entry
 * \param[in]   index   index in directory
 *
 * \return  false on invalid index
 */
bool cbmfm_t64_dirent_parse(cbmfm_t64_t *image,
                            cbmfm_dirent_t *dirent,
                            uint16_t index)
{
    uint8_t *data;
    cbmfm_dirent_t64_t *extra = &(dirent->extra.t64);

    if (index >= image->entry_used) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return false;
    }

    cbmfm_t64_dirent_init(dirent);
    data = image->data + CBMFM_T64_DIR_OFFSET + index * CBMFM_T64_DIRENT_SIZE;

    memcpy(dirent->filename, data + CBMFM_T64_DIRENT_FILE_NAME,
            CBMFM_CBMDOS_FILE_NAME_LEN);

    /* TODO: calculate filesize AFTER fixing the possibly broken end address */
    dirent->filesize = 0;
    dirent->size_blocks = 0;

    /* file type: either PRG or FRZ (which gets set to USR) */
    extra->c64s_type = data[CBMFM_T64_DIRENT_C64S_TYPE];
    dirent->filetype = extra->c64s_type == 3 ? 0x84 : 0x82;

    dirent->image = (cbmfm_image_t *)image;

    extra->load_addr = cbmfm_word_get_le(data + CBMFM_T64_DIRENT_LOAD_ADDR);
    extra->end_addr = cbmfm_word_get_le(data + CBMFM_T64_DIRENT_END_ADDR);
    extra->data_offset = cbmfm_dword_get_le(data + CBMFM_T64_DIRENT_DATA_OFFSET);

    /* TODO: first fix any corruption in the dirent */
    dirent->size_blocks = cbmfm_size_to_blocks(
            (size_t)(extra->end_addr - extra->load_addr + 2));

    return true;
}


/** @} */
