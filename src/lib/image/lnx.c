/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/lnx.c
 * \brief   Lynx archive handling
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
#include <ctype.h>

#include "cbmfm_types.h"
#include "base.h"
#include "base/image.h"
#include "base/dir.h"
#include "base/dirent.h"
#include "base/file.h"
#include "log.h"

#include "lnx.h"


/** \ingroup    lib_archive_lnx
 * @{
 */


/** \brief  Parse a base-10 integer value
 *
 * \param[in]   s       string to parse
 * \param[out]  endptr  object to store pointer to first non-valid char
 *
 * \return  value or -1 on error
 *
 * \throw   #CBMFM_ERR_INVALID_DATA
 */
static intmax_t cbmfm_parse_int(char *s, char **endptr)
{
    intmax_t result = 0;
    char *t = s;

    *endptr = s;    /* for errors */

    /* skip leading whitespace */
    while (*t != '\0' && isspace(*t)) {
        t++;
    }

    if (*t == '\0') {
        /* no data, parse error */
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        return -1;
    }

    while (*t != '\0' && isdigit(*t)) {
        result = result * 10 + *t - '0';
        t++;
    }
    *endptr = t;
    return result;
}


/** \brief  Parse header of \a image
 *
 * \param[in,out]   image   Lynx image
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INVALID_DATA
 */
static bool lnx_parse_header(cbmfm_lnx_t *image)
{
    uint8_t *data = image->data;
    intmax_t dir_size = 0;
    intmax_t dir_used = 0;
    char *endptr = NULL;
    char *s;
    int i;

    /* get dir size in blocks */
    dir_size = cbmfm_parse_int((char *)(data + CBMFM_LNX_HDR), &endptr);
    if (dir_size < 0) {
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        return false;
    }
    image->dir_blocks = (uint16_t)dir_size;

    /* get version string */
    memset(image->version_str, 0x00, 32);
    s = endptr;
    while (*s != '\0' && isspace(*s)) {
        s++;
    }
    i = 0;
    while (*s != 0x0d && i < 31) {
        image->version_str[i] = *s;
        i++;
        s++;
    }

    dir_used = cbmfm_parse_int(s, &endptr);
    if (dir_used < 0) {
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        return false;
    }
    image->dir_used = (uint16_t)dir_used;

    /* determine start of actual directory */
    s = endptr;
    while (isspace(*s) && ((uint8_t *)s < image->data + image->size)) {
        s++;
    }
    image->dir_start = (uint8_t *)s;
    return true;
}


/** \brief  Initialize Lynx dirent
 *
 * \param[out]  dirent  dirent
 *
 */
static void cbmfm_lnx_dirent_init(cbmfm_dirent_t *dirent)
{
    cbmfm_dirent_init(dirent);
    dirent->extra.lnx.remainder = 0;
    dirent->image_type = CBMFM_IMAGE_TYPE_LNX;
}


/** \brief  Parse Lynx dirent
 *
 * \param[out]  dirent  directory entry
 * \param[in]   data    dirent data
 *
 * \return  number of bytes parsed, or -1 on failure
 *
 * \throw   #CBMFM_ERR_INVALID_DATA
 */
static int cbmfm_lnx_dirent_parse(cbmfm_dirent_t *dirent, uint8_t *data)
{
    intmax_t blocks;
    intmax_t remainder;
    char *endptr;
    char *s;

    cbmfm_lnx_dirent_init(dirent);

    /* PETSCII file name */
    memcpy(dirent->filename, data, CBMFM_CBMDOS_FILE_NAME_LEN);

    /* file size in blocks */
    s = (char *)(data + CBMFM_CBMDOS_FILE_NAME_LEN);
    blocks = cbmfm_parse_int(s, &endptr);
    if (blocks < 0) {
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        return -1;
    }
    dirent->size_blocks = (uint16_t)blocks;

    /* file type */
    s = endptr;
    while (isspace(*s)) {
        s++;
    }
    switch (*s) {
        case 0x50:
            /* PRG */
            dirent->filetype = CBMFM_CBMDOS_PRG;
            break;
        case 0x52:
            /* REL */
            dirent->filetype = CBMFM_CBMDOS_REL;
            break;
        case 0x53:
            /* SEQ */
            dirent->filetype = CBMFM_CBMDOS_SEQ;
            break;
        case 0x55:
            /* USR */
            dirent->filetype = CBMFM_CBMDOS_USR;
            break;
        default:
            /* invalid file type */
            cbmfm_errno = CBMFM_ERR_INVALID_DATA;
            return -1;
    }
    dirent->filetype |= CBMFM_CBMDOS_FILE_CLOSED_BIT;

    s++;

    /* get remainder in final block */
    remainder = cbmfm_parse_int(s, &endptr);
    if (remainder < 0) {
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        return -1;
    }
    dirent->extra.lnx.remainder = (uint8_t)remainder;

    /* determine file size */
    dirent->filesize = (size_t)(CBMFM_BLOCK_SIZE_DATA * (blocks - 1) + remainder);

    /* skip trailing whitespace */
    s = endptr;
    while (isspace(*s)) {
        s++;
    }

    return (int)((uint8_t *)s - data);
}


/** \brief  Allocate Lynx image
 *
 * \return  heap-allocated Lynx image
 */
cbmfm_lnx_t *cbmfm_lnx_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_lnx_t));
}


/** \brief  Initialize \a image for use
 *
 * \param[out]  image   Lynx image
 */
void cbmfm_lnx_init(cbmfm_lnx_t *image)
{
    cbmfm_image_init((cbmfm_image_t *)image);
    /* Lynx specific init */
    image->type = CBMFM_IMAGE_TYPE_LNX;
    image->dir_start = NULL;
    image->dir_blocks = 0;
    image->dir_used = 0;
}


/** \brief  Allocate and initialize a Lynx image
 *
 * \return  new Lynx image
 */
cbmfm_lnx_t *cbmfm_lnx_new(void)
{
    cbmfm_lnx_t *image = cbmfm_lnx_alloc();
    cbmfm_lnx_init(image);
    return image;
}


/** \brief  Free all members of \a image, but not \a image itself
 *
 * \param[in,out]   image   Lynx image
 */
void cbmfm_lnx_cleanup(cbmfm_lnx_t *image)
{
    cbmfm_image_cleanup((cbmfm_image_t *)image);
    /* TODO: Lynx-specific cleanup, if any */
}


/** \brief  Free all members of \a image and \a image itself
 *
 * \param[in,out]   image   Lynx image
 */
void cbmfm_lnx_free(cbmfm_lnx_t *image)
{
    cbmfm_lnx_cleanup(image);
    cbmfm_free(image);
}


/** \brief  Read data from \a path into \a image
 *
 * \param[in,out]   image   Lynx image
 * \param[in]       path    path to image file
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_IO
 * \throw   #CBMFM_ERR_INVALID_DATA
 */
bool cbmfm_lnx_open(cbmfm_lnx_t *image, const char *path)
{
    uint8_t *data;
    intmax_t size;

    size = cbmfm_read_file(&data, path);
    if (size < 0) {
        return false;
    }

    /* check size */
    if (size < CBMFM_LNX_MIN_SIZE) {
        cbmfm_errno = CBMFM_ERR_SIZE_MISMATCH;
        cbmfm_free(data);
        return false;
    }

    /* check load address */
    if (data[0] != (CBMFM_LNX_LOAD_ADDR & 0xff)
            || data[1] != ((CBMFM_LNX_LOAD_ADDR >> 8) & 0xff)) {
        cbmfm_errno = CBMFM_ERR_INVALID_DATA;
        cbmfm_free(data);
        return false;
    }

    /* OK: */
    image->data = data;
    image->size = (size_t)size;
    image->path = cbmfm_strdup(path);

    if (!lnx_parse_header(image)) {
        cbmfm_lnx_cleanup(image);
        return false;
    }
    return true;
}


/** \brief  Dump some info on \a image on stdout
 *
 * \param[in]   image   Lynx image
 */
void cbmfm_lnx_dump(const cbmfm_lnx_t *image)
{
    printf("version string   : '%s'\n", image->version_str);
    printf("directory blocks : %u\n", (unsigned int)image->dir_blocks);
    printf("directory entries: %u\n", (unsigned int)image->dir_used);
    printf("directory offset : %zu\n",
            (size_t)(image->dir_start - image->data));
}


/** \brief  Read directory of \a image
 *
 * \param[in]   image   Lynx image
 *
 * \return  heap-allocated directory or `NULL` on failure
 *
 * \throw   #CBMFM_ERR_INVALID_DATA
 */
cbmfm_dir_t *cbmfm_lnx_dir_read(cbmfm_lnx_t *image)
{
    cbmfm_dir_t *dir;
    cbmfm_dirent_t dirent;
    uint8_t *data;
    uint16_t index;

    dir = cbmfm_dir_new();
    data = image->dir_start;

    for (index = 0; index < image->dir_used; index++) {
        int len = cbmfm_lnx_dirent_parse(&dirent, data);
        if (len < 0) {
            cbmfm_dir_free(dir);
            return NULL;
        }
        dirent.index = index;
        cbmfm_dir_append_dirent(dir, &dirent);
        data += len;
    }
    dir->image = (cbmfm_image_t *)image;
    return dir;
}


/** \brief  Read file from Lynx dir
 *
 * Reads file data from a Lynx image via \a dir and stores it in \a file. The
 * reason for using a directory rather than directly accessing the Lynx image
 * has to do with the incredibly crappy directory structure of Lynx images.
 *
 * \param[in]   dir     Lynx directory
 * \param[out]  file    file object
 * \param[in]   index   index in \a dir
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INDEX
 */
bool cbmfm_lnx_file_read(cbmfm_dir_t *dir, cbmfm_file_t *file, uint16_t index)
{
    cbmfm_lnx_t *image;
    cbmfm_dirent_t *dirent;
    uint16_t idx;
    uint8_t *data;
    size_t filesize;

    if (index >= dir->entry_used) {
        cbmfm_errno = CBMFM_ERR_INDEX;
        return false;
    }

    image = (cbmfm_lnx_t *)(dir->image);

    /* get pointer to file data section */
    data = image->data + CBMFM_BLOCK_SIZE_DATA * image->dir_blocks;

    cbmfm_file_init(file);

    idx = 0;
    while (idx < index) {
        dirent = dir->entries[idx];

        data += dirent->size_blocks * CBMFM_BLOCK_SIZE_DATA;
        idx++;
    }
    dirent = dir->entries[idx];

    cbmfm_log_debug("offset in image = %zu\n", (size_t)(data - image->data));

    /*
     * Fix bugged entries: seems some Lynx containers have a 'remainder' value
     * that is one byte to large, making reading data from a final entry
     * read past valid memory.
     */
    filesize = dirent->filesize;
    if ((size_t)(data - image->data) + filesize > image->size) {
        cbmfm_log_warning("adjusting file size from %zu to %zu\n",
                filesize, image->size - (size_t)(data - image->data));
        filesize = image->size - (size_t)(data - image->data);
    }

    /* store data in file object */
    file->size = filesize;
    file->data = cbmfm_memdup(data, filesize);
    memcpy(file->name, dirent->filename, CBMFM_CBMDOS_FILE_NAME_LEN);
    file->type = dirent->filetype;

    return true;
}


/** \brief  Extract file from Lynx archive and write to host file system
 *
 * \param[in]   dir         Lynx directory
 * \param[in]   index       index in directory of file
 * \param[in]   filename    filename (use `NULL` to use PETSCII filename)
 *
 * \return  bool
 *
 * \throw   #CBMFM_ERR_INDEX
 */
bool cbmfm_lnx_file_extract(cbmfm_dir_t *dir,
                            uint16_t index,
                            const char *filename)
{
    cbmfm_file_t file;
    bool result;

    /* read fiile data */
    if (!cbmfm_lnx_file_read(dir, &file, index)) {
        return false;
    }

    /* write to host file system */
    result = cbmfm_file_write_host(&file, filename);
    cbmfm_file_cleanup(&file);
    return result;
}
