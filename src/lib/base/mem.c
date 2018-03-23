/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/mem.c
 * \brief   Base library memory allocation handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * This module handles memory allocation. It currently implements a simple
 * succeed-or-die (ie xmalloc) approach. Since the memory allocated by this
 * application/lib will be relatively little, and Gtk3/GLib will also exit
 * on OOM, this seemed like a decent approach.
 *
 * Should OOM become a problem, the wrappers can be rewritten to use
 * 'free lists', marking something as reusable while not actually freeing it,
 * but freeing those items in case of an OOM condition and retrying a malloc()
 * call (though this seems like overkill to me).
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
#include <string.h>

#include "errors.h"

#include "mem.h"


/** \brief  Allocate \a size bytes of memory
 *
 * This function uses a succeed-or-die approach, since both GLib and Gtk+ use
 * that as well.
 *
 * \param[in]   size    number of bytes to allocate
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
 * \note    Do not use when trying to realloc \a ptr to a smaller size. The
 *          standard guarantees the original \a ptr is still intact when
 *          requesting a smaller size fails, but this function barfs.
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


/** \brief  Reallocate memory at \a ptr to \a size bytes
 *
 * Only use this function to realloc \a ptr to a smaller size. Since
 * cbfm_realloc() fails when realloc(3) cannot find a smaller slot, this
 * function is needed to try to reallocate a chunk of memory to a smaller size.
 * The C Standard guarantees the original \a ptr is intact when realloc(3)
 * fails, this function cannot fail. (Just don't pass `NULL` as its first
 * argument, just use cbmfm_malloc() for that).
 *
 * \param[in,out]   ptr     memory to reallocate
 * \param[in]       size    new size of memory at \a ptr
 *
 * \note    This function is only meant to be used when requesting a smaller
 *          size for \a ptr. If realloc(3) fails, the original \a ptr is
 *          returned with its original size, if realloc(3) succeeds, a pointer
 *          to the resized memory is returned.
 *
 * \return  pointer to reallocated memory
 */

void *cbmfm_realloc_smaller(void *ptr, size_t size)
{
    void *tmp = realloc(ptr, size);
    if (tmp == NULL) {
        /* failed to get smaller slot, return original */
        return ptr;
    }
    /* got smaller slot */
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


/** \brief  Create a heap-allocated copy of at most \a n bytes of \a s
 *
 * \param[in]   s   string to copy
 * \param[in]   n   maximum number of bytes to copy
 *
 * \return  heap-allocated, nul-terminated string
 */
char *cbmfm_strndup(const char *s, size_t n)
{
    const char *orig;
    char *t;

    orig = s;
    t = cbmfm_malloc(n + 1);

    while (*s != '\0' && ((size_t)(s - orig) < n)) {
        *t = *s;
        t++;
        s++;
    }
    *t = '\0';

    return t;
}


/** \brief  Create a heap-allocated copy of \a s
 *
 * \param[in]   s   string to copy
 *
 * \return  heap-allocated copy of \a s
 */
char *cbmfm_strdup(const char *s)
{
    char *t;
    size_t n;

    n = strlen(s);
    t = cbmfm_malloc(n + 1);
    memcpy(t, s, n + 1);    /* also copy nul-character */
    return t;
}


