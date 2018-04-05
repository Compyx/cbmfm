/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/log.h
 * \brief   Logging facility - header
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

#ifndef CBMFM_LIB_LOG_H
#define CBMFM_LIB_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/** \brief  Log level enumeration
 */
typedef enum cbmfm_log_level_e {
    CBMFM_LOG_LEVEL_NONE,       /**< don't log anything */
    CBMFM_LOG_LEVEL_ERROR,      /**< errors */
    CBMFM_LOG_LEVEL_WARNING,    /**< warning and errors */
    CBMFM_LOG_LEVEL_INFO,       /**< info, warning and errors */
    CBMFM_LOG_LEVEL_DEBUG       /**< debug, info warning and errors */
} cbmfm_log_level_t;


void cbmfm_log_set_level(cbmfm_log_level_t level);
bool cbmfm_log_set_file(const char *path);
void cbmfm_log_close(void);

void cbmfm_log_message(cbmfm_log_level_t level, const char *fmt, ...);

#endif
