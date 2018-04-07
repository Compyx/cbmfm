/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   petasc.h
 * \brief   PETSCII to ASCII conversion and vice versa - header
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

#ifndef CBFMFM_LIB_BASE_PETASC_H
#define CBFMFM_LIB_BASE_PETASC_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


uint8_t cbmfm_pet_to_asc(uint8_t pet);
uint8_t cbmfm_asc_to_pet(uint8_t asc);
bool    cbmfm_is_host_allowed_char(int ch);
void    cbmfm_pet_to_asc_str(char *asc, const uint8_t *pet, size_t n);
void    cbmfm_asc_to_pet_str(uint8_t *pet, const char *asc, size_t n);
void    cbmfm_pet_filename_to_host(char *asc, const uint8_t *pet, const char *ext);
int     cbmfm_write_petscii_digits(uint8_t *pet, int value, size_t len);
char *  cbmfm_basename(char *path);

#endif
