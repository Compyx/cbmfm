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
#include "base/dirent.h"
#include "base/file.h"

#include "ark.h"


/*
 * ARK: private methods
 */

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
    sectors = required / CBMFM_BLOCK_SIZE_DATA;
    if (required % CBMFM_BLOCK_SIZE_DATA != 0) {
        sectors++;
    }

    return sectors * CBMFM_BLOCK_SIZE_DATA;
}


/** \brief  Get number of 'directory' entries in \a image
 *
 * \param[in]   image   Ark image
 *
 * \return  number of entries
 */
static int ark_dirent_count(const cbmfm_image_t *image)
{
    return image->data[CBMFM_ARK_DIRENT_COUNT];
}


/** \brief  Check if file \a index is valid for \a image
 *
 * \param[in]   image   Ark image
 * \param[in]   index   file index
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INDEX
 */
static bool ark_file_index_check(const cbmfm_image_t *image, int index)
{
    if (index < 0 || index >= ark_dirent_count(image)) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return false;
    }
    return true;
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
    if (!ark_file_index_check(image, index)) {
        /* error code already set */
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
    uint8_t *data = image->data + ark_file_data_offset(image);

    if (!ark_file_index_check(image, index)) {
        return NULL;
    }

    while (ent_idx < index) {
        size_t blocks;
        uint8_t *entry;

        entry = ark_dirent_ptr(image, ent_idx);
        blocks = (size_t)(entry[CBMFM_ARK_DIRENT_FILESIZE]
                + entry[CBMFM_ARK_DIRENT_FILESIZE + 1]);

        data += (blocks * CBMFM_BLOCK_SIZE_DATA);

        ent_idx++;
    }
    return data;
}


#if 0
static size_t ark_file_data_size(cbmfm_image_t *image, int index)
{
    uint8_t *dirent = ark_dirent_ptr(image, index);
    size_t blocks;

    blocks = (size_t)(dirent[CBMFM_ARK_DIRENT_FILESIZE]
            + dirent[CBMFM_ARK_DIRENT_FILESIZE + 1] * 256);

    return (blocks - 1) * CBMFM_SECTOR_SIZE_DATA
        + dirent[CBMFM_ARK_DIRENT_LAST_SEC_USED] - 1;
}
#endif


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
    dirent->image_type = CBMFM_IMAGE_TYPE_ARK;

    /* get CBMDOS filename */
    memcpy(dirent->filename, data + CBMFM_ARK_DIRENT_FILENAME,
            CBMFM_CBMDOS_FILE_NAME_LEN);

    /* calculate file size */
    blocks = (size_t)(data[CBMFM_ARK_DIRENT_FILESIZE]
            + data[CBMFM_ARK_DIRENT_FILESIZE + 1] * 256);

    /* the LAST_SEC_USED byte indicates the number of bytes in the final
     * sector + 1, for some reason */
    dirent->filesize = (blocks - 1) * CBMFM_BLOCK_SIZE_DATA
        + data[CBMFM_ARK_DIRENT_LAST_SEC_USED] - 1;

    dirent->size_blocks = (uint16_t)blocks;

    /* set CBMDOS file type */
    dirent->filetype = data[CBMFM_ARK_DIRENT_FILETYPE];

    /* store reference to parent image */
    dirent->image = image;
    return true;
}


/*
 * ARK: public methods
 */

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
    printf("filesize        : $%04zx\n", image->size);
    printf("dir entries     : %d\n", ark_dirent_count(image));
    printf("file data offset: $%04zx\n", ark_file_data_offset(image));
}


/** \brief  Read directory of \a image and return a new dir object
 *
 * Read directory of \a image, optionally reading each entry's file data into
 * its dirent.
 *
 * \param[in]   image           ARK image
 * \param[in]   read_file_data  read data of each file into its dirent
 *
 * \return  new dir object
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
        dirent.index = (uint16_t)index;
        cbmfm_dir_append_dirent(dir, &dirent);
        cbmfm_dirent_cleanup(&dirent);
    }

    /* store reference to parent image */
    dir->image = image;
    return dir;
}


/** \brief  Read file from \a image into \a file object
 *
 * \param[in]   image   ARK archive
 * \param[out]  file    file object
 * \param[in]   index   index in archive
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INDEX
 */
bool cbmfm_ark_read_file(cbmfm_image_t *image, cbmfm_file_t *file, int index)
{
    cbmfm_dirent_t dirent;

    if (!ark_file_index_check(image, index)) {
        return false;
    }

    /* grab the dirent */
    cbmfm_dirent_init(&dirent);
    ark_parse_dirent(image, &dirent, index);

    /* copy data to file object */
    memcpy(file->name, dirent.filename, CBMFM_CBMDOS_FILE_NAME_LEN);
    file->data = cbmfm_malloc(dirent.filesize);
    memcpy(file->data, ark_file_data_ptr(image, index), dirent.filesize);
    file->size = dirent.filesize;
    file->type = dirent.filetype;

    return true;
}


/** \brief  Extract file at \a index from \a image
 *
 * \param[in]   image   Ark image
 * \param[in]   name    file name (optional, use `NULL` to use the CBMDOS name)
 * \param[in]   index   index of file in \a image
 *
 * \return  bool
 */
bool cbmfm_ark_extract_file(cbmfm_image_t *image, const char *name, int index)
{
    cbmfm_file_t file;
    bool result;

    if (!cbmfm_ark_read_file(image, &file, index)) {
        return false;
    }

    result = cbmfm_file_write_host(&file, name);
    cbmfm_file_cleanup(&file);

    return result;
}


/** \brief  Extract all files in \a image
 *
 * Extracts all files in \a image and writes them using their PETSCII file
 * names converted to the host file system encoding (ASCII).
 *
 * \param[in]   image   Ark image
 *
 * \return  bool
 */
bool cbmfm_ark_extract_all(cbmfm_image_t *image)
{
    int index;

    for (index = 0; index < ark_dirent_count(image); index++) {
        if (!cbmfm_ark_extract_file(image, NULL, index)) {
            return false;
        }
    }
    return true;
}
