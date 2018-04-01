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


void cbmfm_block_init(cbmfm_block_t *block);


cbmfm_dirent_t *cbmfm_dirent_alloc(void);
void            cbmfm_dirent_init(cbmfm_dirent_t *dirent);
void            cbmfm_dirent_cleanup(cbmfm_dirent_t *dirent);
void            cbmfm_dirent_free(cbmfm_dirent_t *dirent);

#endif
