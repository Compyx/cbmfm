/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/mem.c
 * \brief   Base library memory allocation handling
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
#include <inttypes.h>

#include "errors.h"

#include "mem.h"


/** \brief  Allocate \a n bytes of memory
 *
 * This function uses a succeed-or-die approach, since both GLib and Gtk+ use
 * that as well.
 *
 * \param[in]   n   number of bytes to allocate
 *
 * \return  pointer to allocated memory
 */
void *cbmfm_malloc(size_t size)
{
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr,
                "cbmfm_malloc(): failed to allocate %zu bytes, exiting\n",
                size);
        exit(1);
    }
    return ptr;
}


/** \brief  Allocate and zero-out memory
 *
 * \param[in]   nmemb   number of elements
 * \param[in]   size    size of elements
 *
 * \return  pointer to allocated memory
 */
void *cbmfm_calloc(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fprintf(stderr,
                "cbmfm_calloc(): failed to allocate %zu bytes, exiting\n",
                size);
        exit(1);
    }
    return ptr;
}


/** \brief  Reallocate memory at \a ptr to \a size bytes
 *
 * \param[in,out]   ptr     memory to reallocate
 * \param[in]       size    new size of memory at \a ptr
 *
 * \return  pointer to reallocated memory
 */
void *cbmfm_realloc(void *ptr, size_t size)
{
    void *tmp = realloc(ptr, size);
    if (tmp == NULL) {
        fprintf(stderr,
                "cbmfm_realloc(): failed to allocate %zu bytes, exiting\n",
                size);
        exit(1);
    }
    return tmp;
}


/** \brief  Free memory
 *
 * \param[in,out]   ptr pointer to memory to free
 */
void cbmfm_free(void *ptr)
{
    free(ptr);
}
