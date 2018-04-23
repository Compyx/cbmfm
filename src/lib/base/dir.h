/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/dir.h
 * \brief   Directory handling - header
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

#ifndef CBMFM_LIB_BASE_DIR_H
#define CBMFM_LIB_BASE_DIR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "cbmfm_types.h"


cbmfm_dir_t *   cbmfm_dir_alloc(void);
void            cbmfm_dir_init(cbmfm_dir_t *dir);
cbmfm_dir_t *   cbmfm_dir_new(void);
void            cbmfm_dir_cleanup(cbmfm_dir_t *dir);
void            cbmfm_dir_free(cbmfm_dir_t *dir);
void            cbmfm_dir_append_dirent(cbmfm_dir_t *dir,
                                        const cbmfm_dirent_t *dirent);

void            cbmfm_dir_dump(const cbmfm_dir_t *dir);

bool            cbmfm_dxx_dir_iter_init(cbmfm_dxx_dir_iter_t *iter,
                                        cbmfm_dxx_image_t *image,
                                        int track, int sector);
bool            cbmfm_dxx_dir_iter_next(cbmfm_dxx_dir_iter_t *iter);
uint8_t *       cbmfm_dxx_dir_iter_entry_ptr(cbmfm_dxx_dir_iter_t *iter);



#endif
