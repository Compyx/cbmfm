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
#include <string.h>

#include "cbmfm_types.h"
#include "base.h"
#include "base/image.h"
#include "base/dir.h"

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


/** \brief  Dump same stats on ARK \a image
 *
 * Debugging function: dump some data about \a image on stdout.
 *
 * \param[in]   image   ARK image
 */
void cbmfm_ark_dump_stats(const cbmfm_image_t *image)
{
    printf("filename        : '%s'\n", image->path);
    printf("filesize        : $%04lx\n", (unsigned long)(image->size));
    printf("dir entries     : %u\n", image->data[0]);    /* FIXME */
    printf("file data offset: $%04lx\n", (unsigned long)ark_file_data_offset(image));
}


/** \brief  Get number of 'directory' entries in \a image
 *
 * \return  number of entries
 */
static int ark_dirent_count(const cbmfm_image_t *image)
{
    return image->data[CBMFM_ARK_DIRENT_COUNT];
}


/** \brief  Get pointer to dirent data at \a index in \a image
 *
 * \param[in]   image   ARK image
 * \param[in]   index   index in 'directory' of \a image
 *
 * \return  pointer to dirent data or `NULL` on index error
 *
 * \throw   #CBMFM_ERR_INDEX
 */
static uint8_t *ark_dirent_ptr(cbmfm_image_t *image, int index)
{
    if (index < 0 || index >= ark_dirent_count(image)) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return NULL;
    }

    return image->data + CBMFM_ARK_DIR_OFFSET + index * CBMFM_ARK_DIRENT_SIZE;
}


/** \brief  Get pointer to file data of file \a index in \a image
 *
 * \param[in]   image   ARK image
 * \param[in]   index   file index in the 'directory' of \a image
 *
 * \return  pointer to file data or `NULL` on index error
 *
 * \throw   #CBMFM_ERR_INDEX
 */
static uint8_t *ark_file_data_ptr(cbmfm_image_t *image, int index)
{
    int ent_idx = 0;
    int ent_cnt = ark_dirent_count(image);
    uint8_t *data = image->data + ark_file_data_offset(image);


    if (index < 0 || index >= ent_cnt) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return NULL;
    }

    while (ent_idx < index) {
        size_t blocks;
        uint8_t *entry;

        entry = ark_dirent_ptr(image, ent_idx);
        blocks = (size_t)(entry[CBMFM_ARK_DIRENT_FILESIZE]
                + entry[CBMFM_ARK_DIRENT_FILESIZE + 1]);

        data += (blocks * CBMFM_SECTOR_SIZE_DATA);

        ent_idx++;
    }
    return data;
}


/** \brief  Parse the 'directory entry at \a index on \a image into \a dirent
 *
 * \param[in]       image   ARK image
 * \param[in,out]   dirent  dir object to store data
 * \param[in]       index   index in the 'directory' of \a image
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INDEX
 */
static bool ark_parse_dirent(
        cbmfm_image_t *image,
        cbmfm_dirent_t *dirent,
        int index)
{
    size_t blocks;
    uint8_t *data = ark_dirent_ptr(image, index);

    /* index error, already set */
    if (data == NULL) {
        return false;
    }

    /* initialize dirent */
    cbmfm_dirent_init(dirent);

    /* get CBMDOS filename */
    memcpy(dirent->filename, data + CBMFM_ARK_DIRENT_FILENAME,
            CBMFM_CBMDOS_FILENAME_LEN);

    /* calculate file size */
    blocks = (size_t)(data[CBMFM_ARK_DIRENT_FILESIZE]
            + data[CBMFM_ARK_DIRENT_FILESIZE + 1] * 256);
    dirent->filesize = (blocks - 1) * CBMFM_SECTOR_SIZE_DATA
        + data[CBMFM_ARK_DIRENT_LAST_SEC_USED];
    dirent->size_blocks = (uint16_t)blocks;

    /* set CBMDOS file type */
    dirent->filetype = data[CBMFM_ARK_DIRENT_FILETYPE];

    /* store reference to parent image */
    dirent->image = image;
    return true;
}


/** \brief  Read directory of \a image and return a new dir object
 *
 * \param[in]   image   ARK image
 *
 * \return  new dir object
 *
 * \todo    add support for reading each file's data into its dirent
 */
cbmfm_dir_t *cbmfm_ark_read_dir(cbmfm_image_t *image, bool read_file_data)
{
    cbmfm_dir_t *dir;
    cbmfm_dirent_t dirent;
    int index = 0;

    dir = cbmfm_dir_new();

    for (index = 0; index < ark_dirent_count(image); index++) {
        ark_parse_dirent(image, &dirent, index);
        /* read file data into dirent */
        if (read_file_data) {
            uint8_t *data = ark_file_data_ptr(image, index);
#if 0
            printf("file data offset = %ld\n", (long)(data - image->data));
            printf("file data size = %zu\n", dirent.filesize);
#endif
            if (data != NULL) {
                dirent.filedata = cbmfm_memdup(data, dirent.filesize);
            }
        }
        cbmfm_dir_append_dirent(dir, &dirent);
        cbmfm_dirent_cleanup(&dirent);
    }

    /* store reference to parent image */
    dir->image = image;
    return dir;
}
