/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/image/d64.h
 * \brief   D64 disk image handling - header
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


#ifndef CBMFM_LIB_IMAGE_D64_H
#define CBMFM_LIB_IMAGE_D64_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cbmfm_types.h"
#include "base/dxx.h"


cbmfm_d64_t *   cbmfm_d64_alloc(void);
void            cbmfm_d64_init(cbmfm_d64_t *image);
cbmfm_d64_t *   cbmfm_d64_new(void);
void            cbmfm_d64_cleanup(cbmfm_d64_t *image);
void            cbmfm_d64_free(cbmfm_d64_t *image);

bool            cbmfm_d64_open(cbmfm_d64_t *image, const char *name);

#endif
