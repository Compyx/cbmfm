/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/lib/base/errors.h
 * \brief   Base library error handling - header
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

#ifndef CBMFM_LIB_BASE_ERRORS_H
#define CBMFM_LIB_BASE_ERRORS_H

extern int cbmfm_errno;


/** \brief  Error codes
 */
typedef enum cbmfm_err_e {
    CBMFM_ERR_OK = 0,           /**< no error */
    CBMFM_ERR_OOM,              /**< out of memory error */
    CBMFM_ERR_IO,               /**< I/O error */
    CBMFM_ERR_FILE_TOO_LARGE,   /**< file too large (> 2GB) */
    CBMFM_ERR_NOT_FOUND,        /**< entry/tune not found */
    CBMFM_ERR_INVALID,          /**< invalid data or operation detected */
    CBMFM_ERR_INDEX,            /**< index error */
    CBMFM_ERR_SIZE_MISMATCH,    /**< size mismatch */

    CBMFM_ERR_CODE_COUNT        /**< number of error messages */

} cbmfm_err_t;

const char *cbmfm_strerror(int code);
void        cbmfm_perror(const char *prefix);

#endif
