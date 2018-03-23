/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/image.h
 * \brief   Base image functions - header
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

#ifndef CBMFM_LIB_BASE_IMAGE_H
#define CBMFM_LIB_BASE_IMAGE_H

#include "../cbmfm_types.h"


const char *    cbmfm_image_type_get_ext(cbmfm_image_type_t type);
const char *    cbmfm_image_type_get_desc(cbmfm_image_type_t type);

/*
 * Image creation/destruction functions
 */

void            cbmfm_image_init(cbmfm_image_t *image);
cbmfm_image_t * cbmfm_image_alloc(void);
void            cbmfm_image_cleanup(cbmfm_image_t *image);
void            cbmfm_image_free(cbmfm_image_t *image);

bool cbmfm_image_read_data(cbmfm_image_t *image, const char *path);

/*
 * Image state functions
 */

bool            cbmfm_image_get_readonly(const cbmfm_image_t *image);
void            cbmfm_image_set_readonly(cbmfm_image_t *image, bool readonly);
bool            cbmfm_image_get_dirty(const cbmfm_image_t *image);
void            cbmfm_image_set_dirty(cbmfm_image_t *image, bool dirty);

#endif


