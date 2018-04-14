/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/file.c
 * \brief   CBM file handling
 *
 * This module contains functions to handle CBM files in a flexible manner.
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
#include "base/io.h"
#include "log.h"

#include "file.h"


/** \brief  CBMDOS file type strings
 */
static const char *filetype_str[8] = {
    "del",  /* deleted */
    "seq",  /* sequential */
    "prg",  /* program */
    "usr",  /* user defined */
    "rel",  /* relative */
    "dir",  /* directory/partition (d81) */
    "???",
    "???"
};


/** \brief  Get CBMDOS filetype string
 *
 * \param[in]   filetype    CBMDOS filetype byte
 *
 * \return  0-terminated file type in ASCII
 */
const char *cbmfm_cbmdos_filetype(uint8_t filetype)
{
    return filetype_str[filetype & 0x07];
}


/** \brief  Allocate file object
 *
 * \return  new file object
 */
cbmfm_file_t *cbmfm_file_alloc(void)
{
    return cbmfm_malloc(sizeof(cbmfm_file_t));
}


/** \brief  Initialize \a file to a usable state
 *
 * \param[out]  file    file object
 */
void cbmfm_file_init(cbmfm_file_t *file)
{
    memset(file->name, 0x00, CBMFM_CBMDOS_FILE_NAME_LEN);
    file->data = NULL;
    file->size = 0;
    file->type = 0;
}


/** \brief  Allocate and initialize a file object
 *
 * \return  new file object
 */
cbmfm_file_t *cbmfm_file_new(void)
{
    cbmfm_file_t *file = cbmfm_file_alloc();
    cbmfm_file_init(file);
    return file;
}


/** \brief  Free members of \a file but not \a file itself
 *
 * Frees memory used by \a file's members and reinitializes it for new use.
 *
 * \param[in,out]   file    file object
 */
void cbmfm_file_cleanup(cbmfm_file_t *file)
{
    if (file->data != NULL) {
        cbmfm_free(file->data);
    }
    cbmfm_file_init(file);
}


/** \brief  Free members of \a file and \a file itself
 *
 * \param[in,out]   file    file object
 */
void cbmfm_file_free(cbmfm_file_t *file)
{
    cbmfm_file_cleanup(file);
    cbmfm_free(file);
}


/** \brief  Write \a file to host OS
 *
 * If \a name is `NULL`, the PETSCII file name will be converted to the host
 * encoding and an extension added, depending on the CBMDOS file type.
 *
 * \param[in]   file    file object
 * \param[in]   name    name (optional)
 *
 * \return  bool
 */
bool cbmfm_file_write_host(const cbmfm_file_t *file, const char *name)
{
    char hname[CBMFM_CBMDOS_FILE_NAME_LEN + 1];

    if (name == NULL) {
        const char *prefix = cbmfm_cbmdos_filetype(file->type);
        cbmfm_pet_filename_to_host(hname, file->name, prefix);
        name = hname;
    }

    cbmfm_log_debug("writing file as '%s'\n", name);
    return cbmfm_write_file(file->data, file->size, name);
}
