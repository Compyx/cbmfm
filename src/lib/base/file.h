/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/file.h
 * \brief   CBM file handling - header
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

#ifndef CBMFM_LIB_BASE_FILE_H
#define CBMFM_LIB_BASE_FILE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "cbmfm_types.h"


const char *    cbmfm_cbmdos_filetype(uint8_t filetype);
bool            cbmfm_cbmdos_is_closed(uint8_t filetype);
bool            cbmfm_cbmdos_is_locked(uint8_t filetype);

cbmfm_file_t *  cbmfm_file_alloc(void);
void            cbmfm_file_init(cbmfm_file_t *file);
cbmfm_file_t *  cbmfm_file_new(void);
void            cbmfm_file_cleanup(cbmfm_file_t *file);
void            cbmfm_file_free(cbmfm_file_t *file);
bool            cbmfm_file_write_host(const cbmfm_file_t *file,
                                      const char *name);
void            cbmfm_file_dump(const cbmfm_file_t *file);

#endif
