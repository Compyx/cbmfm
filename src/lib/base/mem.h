/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/mem.h
 * \brief   Base library memory allocation handling - header
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

#ifndef CBMFM_LIB_BASE_MEM_H
#define CBMFM_LIB_BASE_MEM_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


void *      cbmfm_malloc(size_t size);
void *      cbmfm_calloc(size_t nmemb, size_t size);
void *      cbmfm_realloc(void *ptr, size_t size);
void *      cbmfm_realloc_smaller(void *ptr, size_t size, bool *success);
void        cbmfm_free(void *ptr);

char *      cbmfm_strdup(const char *s);
char *      cbmfm_strndup(const char *s, size_t n);
void *      cbmfm_memdup(const void *data, size_t size);

int         cbmfm_popcount_byte(uint8_t b);

void        cbmfm_hexdump(const uint8_t *data, size_t skip, size_t size);

uint16_t    cbmfm_word_get_le(const uint8_t *p);
void        cbmfm_word_set_le(uint8_t *p, uint16_t v);
uint16_t    cbmfm_word_get_be(const uint8_t *p);
void        cbmfm_word_set_be(uint8_t *p, uint16_t v);
uint32_t    cbmfm_dword_get_le(const uint8_t *p);
void        cbmfm_dword_set_le(uint8_t *p, uint32_t v);
uint32_t    cbmfm_dword_get_be(const uint8_t *p);
void        cbmfm_dword_set_be(uint8_t *p, uint32_t v);

#endif
