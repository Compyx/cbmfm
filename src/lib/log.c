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
#include "base.h"

#include "log.h"



static FILE *log_file = NULL;
static cbmfm_log_level_t log_level = CBMFM_LOG_LEVEL_NONE;


void cbmfm_log_set_level(cbmfm_log_level_t level)
{
    log_level = level;
}

bool cbmfm_log_set_file(const char *path)
{
    log_file = fopen(path, "rb");
    if (log_file == NULL) {
        return false;
    }
    return true;
}

void cbmfm_log_close(void)
{
    if (log_file != NULL) {
        fclose(log_file);
    }
}


void cbmfm_log_message(cbmfm_log_level_t level, const char *fmt, ...)
{
    FILE *fp;
    va_list args;

    va_start(args, fmt);

    if (level <= log_level) {
        fp = log_file != NULL ? log_file : stdout;
        vfprintf(fp, fmt, args);
    }
}

