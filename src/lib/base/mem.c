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
#include <ctype.h>

#include "errors.h"

#include "mem.h"


/** \brief  Popcount table
 *
 * Number of bits for values 0 - 15, used to quickly determine the number of
 * bits in an integer.
 */
static const int popcount_table[16] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4
};




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
 * argument, just use cbmfm_malloc() for that). The \a success argument can
 * be used to see if realloc(3) succeeded or failed, `NULL` can be used to
 * indicate the caller isn't interested.
 *
 * \param[in,out]   ptr     memory to reallocate
 * \param[in]       size    new size of memory at \a ptr
 * \param[out]      success object to store success state (optional)
 *
 * \note    This function is only meant to be used when requesting a smaller
 *          size for \a ptr. If realloc(3) fails, the original \a ptr is
 *          returned with its original size, if realloc(3) succeeds, a pointer
 *          to the resized memory is returned.
 *
 * \return  pointer to reallocated memory
 */
void *cbmfm_realloc_smaller(void *ptr, size_t size, bool *success)
{
    void *tmp = realloc(ptr, size);
    if (tmp == NULL) {
        /* failed to get smaller slot, return original */
        if (success != NULL) {
            *success = false;
        }
        return ptr;
    }
    /* got smaller slot */
    if (success != NULL) {
        *success = true;
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


/** \brief  Create heap-allocated copy of \a size bytes of \a data
 *
 * \param[in]   data    data to copy
 * \param[in]   size    number of bytes to copy of \a data
 *
 * \return  copy of \a size bytes of \a data
 */
void *cbmfm_memdup(const void *data, size_t size)
{
    void *dest = cbmfm_malloc(size);
    memcpy(dest, data, size);
    return dest;
}


/** \brief  Count number of set(1) bits in byte \a b
 *
 * \return  number of set bits in \a b
 */
int cbmfm_popcount_byte(uint8_t b)
{
    return popcount_table[b & 0x0f] + popcount_table[(b >> 4) & 0x0f];
}


/** \brief  Create hexdump of \a data on stdout
 *
 * \param[in]   data    data to dump
 * \param[in]   skip    number of bytes to skip
 * \param[in]   size    number of bytes to dump
 */
void cbmfm_hexdump(const uint8_t *data, size_t skip, size_t size)
{
    size_t offset = skip;
    size_t count = 0;
    size_t col;
    uint8_t text[17];

    text[16] = '\0';
    while (count < size) {
        memset(text, ' ', 16);
        printf("%04x:%04x  ",
                (unsigned int)(offset >> 16),
                (unsigned int)(offset & 0xffff));
        col = 0;
        while (col < 16 && count + col < size) {
            uint8_t b = data[offset + col];

            text[col] = isprint((int)b) ? b : '.';
            printf("%02x ", data[offset + col]);
            col++;
        }
        /* add spacing if last row printed contained less than 16 bytes */
        while (col < 16) {
            printf("   ");
            col++;
        }

        printf("%s\n", text);

        count += 16;
        offset += 16;
    }
}
