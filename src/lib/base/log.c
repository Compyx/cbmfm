/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen : */

/** \file   src/lib/base/log.c
 * \brief   Logging facility
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
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "cbmfm_types.h"

#include "log.h"


/** \brief  Prefixes used in log messages
 */
static const char *log_prefixes[] = {
    "None",
    "Error",
    "Warning",
    "Info",
    "Debug"
};


/** \brief  Log file pointer
 */
static FILE *log_file = NULL;

/** \brief  Log level
 */
static cbmfm_log_level_t log_level = CBMFM_LOG_NONE;


/** \brief  Set log level
 *
 * \param[in]   level   log level
 */
void cbmfm_log_set_level(cbmfm_log_level_t level)
{
    log_level = level;
}


/** \brief  Set log file to \a path
 *
 * \param[in]   path    log file path
 *
 * \return  true on success, false on I/O error
 *
 * \throw   #CBMFM_ERR_IO
 */
bool cbmfm_log_set_file(const char *path)
{
    log_file = fopen(path, "rb");
    if (log_file == NULL) {
        return false;
    }
    return true;
}


/** \brief  Close log file
 */
void cbmfm_log_close(void)
{
    if (log_file != NULL) {
        fclose(log_file);
    }
}


/** \brief  Send a message to the log
 *
 * Send a message to the log, using variable arguments.
 *
 * \param[in]   level   log level
 * \param[in]   fmt     printf format string
 */
void cbmfm_log_message(cbmfm_log_level_t level, const char *fmt, ...)
{
    FILE *fp = log_file != NULL ? log_file : stdout;
    va_list args;

    if (log_level == 0) {
        return;
    }

    if (level <= log_level) {
        fprintf(fp, "%s: ", log_prefixes[level]);
        fflush(fp);

        va_start(args, fmt);

        vfprintf(fp, fmt, args);

        va_end(args);
    }
}
