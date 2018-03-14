/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/io.c
 * \brief   Base library I/O handling
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
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>

#include "errors.h"
#include "mem.h"

#include "io.h"


/** \brief  Size of chunks to read in hvsc_read_file()
 */
#define READFILE_BLOCK_SIZE  65536


/** @brief  Read data from \a path into \a dest, allocating memory
 *
 * This function reads data from \a path, (re)allocating memory as required.
 * The pointer to the result is stored in \a dest. If this function fails for
 * some reason (file not found, out of memory), -1 is returned and all memory
 * used by this function is freed.
 *
 * READFILE_BLOCK_SIZE bytes are read at a time, and whenever memory runs out,
 * it is doubled in size.
 *
 * Example:
 * @code{.c}
 *
 *  uint8_t *data;
 *  intmax_t result;
 *
 *  if ((result = cbmfm_read_file(&data, "Commando.sid")) < 0) {
 *      fprintf(stderr, "oeps!\n");
 *  } else {
 *      printf("OK, read %j bytes\n", result);
 *      free(data);
 *  }
 * @endcode
 *
 * @param   dest    destination of data
 * @param   path    path to file
 *
 * @return  number of bytes read, or -1 on failure
 */
intmax_t cbmfm_read_file(uint8_t **dest, const char *path)
{
    uint8_t *data;
    uint8_t *tmp;
    FILE *fd;
    size_t offset = 0;
    size_t size = READFILE_BLOCK_SIZE;
    size_t result;

    fd = fopen(path, "rb");
    if (fd == NULL) {
        cbmfm_errno = CBMFM_ERR_IO;
        return -1;
    }

    data = cbmfm_malloc(READFILE_BLOCK_SIZE);

    /* keep reading chunks until EOF */
    while (true) {
        /* need to resize? */
        if (offset == size) {
            /* yup */

            /* check limit */
            if (size >= (size_t)INTMAX_MAX + 1) {
                cbmfm_errno = CBMFM_ERR_FILE_TOO_LARGE;
                cbmfm_free(data);
                fclose(fd);
                return -1;
            }

            data = cbmfm_realloc(data, size * 2);
            size *= 2;
        }
        result = fread(data + offset, 1, READFILE_BLOCK_SIZE, fd);
        if (result < READFILE_BLOCK_SIZE) {
            if (feof(fd)) {
                /* OK: EOF */
                /* try to realloc to minimum size required */
                tmp = realloc(data, offset + result);
                if (tmp != NULL) {
                    /* OK, no worries if it fails, the C standard guarantees
                     * the original data is still intact */
                    data = tmp;
                }
                *dest = data;
                fclose(fd);
                return (long)(offset + result);
            } else {
                /* IO error */
                cbmfm_errno = CBMFM_ERR_IO;
                cbmfm_free(data);
                *dest = NULL;
                fclose(fd);
                return -1;
            }
        }
        offset += READFILE_BLOCK_SIZE;
    }
    /* shouldn't get here */
}
