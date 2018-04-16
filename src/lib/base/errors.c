/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/errors.c
 * \brief   Base library error handling
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

#include "errors.h"

/** \brief  Global error code for the library
 */
int cbmfm_errno;


/** \brief  Error messages
 */
static const char *cbmfm_err_messages[CBMFM_ERR_CODE_COUNT] = {
    "OK",
    "out of memory error",
    "I/O error",
    "file too large error",
    "object not found",
    "invalid data",
    "unexpected NULL found",
    "index error",
    "size mismatch",
    "type mismatch",
    "illegal track",
    "illegal sector"
};


/** \brief  Get error message for error code
 *
 * \param[in]   code    error code
 *
 * \return  error message
 */
const char *cbmfm_strerror(int code)
{
    if (code < 0 || code >= CBMFM_ERR_CODE_COUNT) {
        return "<invalid error code>";
    }
    return cbmfm_err_messages[code];
}


/** \brief  Print current error code and message on stderr
 *
 * Prints current library error code and message and stderr, and in case of
 * CBMF_ERR_IO, the C-lib errno and strerror
 *
 * \param[in]   prefix  optional prefix for the message (NULL to disable)
 */
void cbmfm_perror(const char *prefix)
{
    if (prefix != NULL && *prefix != '\0') {
        fprintf(stderr, "%s: ", prefix);
    }
    if (cbmfm_errno == CBMFM_ERR_IO) {
        fprintf(stderr, "%d: %s (%d: %s)\n",
                cbmfm_errno, cbmfm_strerror(cbmfm_errno),
                errno, strerror(errno));
    } else {
        fprintf(stderr, "%d: %s\n", cbmfm_errno, cbmfm_strerror(cbmfm_errno));
    }
}
