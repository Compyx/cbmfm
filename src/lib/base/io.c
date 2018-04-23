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


/** \brief  Size of chunks to read in cbmfm_read_file()
 *
 * 64 KB should be enough for most CBM based files, but obviously things like
 * disk images will use more memory, so perhaps add a function that can set the
 * initial buffer size
 */
#define READFILE_BLOCK_SIZE  (1 << 16)


/** @brief  Read data from \a path into \a dest, allocating memory
 *
 * This function reads data from \a path, (re)allocating memory as required.
 * The pointer to the result is stored in \a dest. If this function fails for
 * some reason (file not found, out of memory), -1 is returned and all memory
 * used by this function is freed.
 *
 * #READFILE_BLOCK_SIZE bytes are read at a time, and whenever memory runs out,
 * it is doubled in size. The initial buffer size is set with \a sizereq.
 *
 * Example:
 * @code{.c}
 *
 *  uint8_t *data;
 *  intmax_t result;
 *
 *  if ((result = cbmfm_read_file(&data, "Commando.sid", 4096)) < 0) {
 *      fprintf(stderr, "oeps!\n");
 *  } else {
 *      printf("OK, read %j bytes\n", result);
 *      free(data);
 *  }
 * @endcode
 *
 * @param   dest    destination of data
 * @param   path    path to file
 * \param   sizereq initial size of buffer
 *
 * @return  number of bytes read, or -1 on failure
 */
intmax_t cbmfm_read_file_sizereq(uint8_t **dest, const char *path, size_t sizereq)
{
    uint8_t *data;
    FILE *fd;
    size_t offset = 0;
    size_t size = sizereq;
    size_t result;

    fd = fopen(path, "rb");
    if (fd == NULL) {
        cbmfm_errno = CBMFM_ERR_IO;
        return -1;
    }

    data = cbmfm_malloc(size);

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
                bool success;
                size_t final_size;

                data = cbmfm_realloc_smaller(data, offset + result, &success);
                *dest = data;
                if (success) {
                    /* realloc succeeded */
                    final_size = offset + result;
                } else {
                    /* realloc failed, return original size */
                    final_size = size;
                }
                fclose(fd);
                return (intmax_t)final_size;
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


/** @brief  Read data from \a path into \a dest, allocating memory
 *
 * This function reads data from \a path, (re)allocating memory as required.
 * The pointer to the result is stored in \a dest. If this function fails for
 * some reason (file not found, out of memory), -1 is returned and all memory
 * used by this function is freed.
 *
 * #READFILE_BLOCK_SIZE bytes are read at a time, and whenever memory runs out,
 * it is doubled in size. The read buffer starts out with a size of
 * #READFILE_BLOCK_SIZE bytes.
 *
 * Example:
 * @code{.c}
 *
 *  uint8_t *data;
 *  intmax_t result;
 *
 *  if ((result = cbmfm_read_file(&data, "Commando.sid", 4096)) < 0) {
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
    return cbmfm_read_file_sizereq(dest, path, READFILE_BLOCK_SIZE);
}


/** \brief  Write \a size bytes of \a data to file \a path
 *
 * \param[in]   data    data to write
 * \param[in]   size    number of bytes to write
 * \param[in]   path    file to write to
 *
 * \return  bool
 */
bool cbmfm_write_file(const uint8_t *data, size_t size, const char *path)
{
    FILE *fp;

    fp = fopen(path, "wb");
    if (fp == NULL) {
        cbmfm_errno = CBMFM_ERR_IO;
        return false;
    }

    if (fwrite(data, 1U, size, fp) != size) {
        cbmfm_errno = CBMFM_ERR_IO;
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}


/** \brief  Determine file size of \a path
 *
 * Determine size of \a path using fseek(3)/ftell(3). This keeps the library
 * code portable, incuring only a small performance penalty.
 *
 * \param[in]   path    path to file
 *
 * \return  size of file in bytes, or -1 on error
 */
long cbmfm_file_size(const char *path)
{
    FILE *fp;
    long result;

    /* open file */
    fp = fopen(path, "rb");
    if (fp == NULL) {
        cbmfm_errno = CBMFM_ERR_IO;
        return -1;
    }

    /* move pointer to end of file */
    if (fseek(fp, 0L, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }

    /* get position */
    result = ftell(fp);
    if (result < 0) {
        cbmfm_errno = CBMFM_ERR_IO;
    }
    fclose(fp);
    return result;
}
