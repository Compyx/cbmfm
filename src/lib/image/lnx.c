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
#include "base/file.h"
#include "log.h"

#include "lnx.h"


/** \ingroup    lib_archive_lnx
 * @{
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
