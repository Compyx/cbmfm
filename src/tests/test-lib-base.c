/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test-lib-base.c
 * \brief   Unit test for src/lib/base
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

#include "base.h"

#include "testcase.h"

static bool test_lib_base_io(struct test_case_s *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_base[] = {
    { "io", "I/O handling", test_lib_base_io, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_base = {
    "base",
    "base library functions",
    tests_lib_base,
    NULL,
    NULL,
    0, 0
};


/** \brief  Test src/lib/base/io.c behaviour
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_base_io(struct test_case_s *test)
{
    puts("\nTesting lib/io/base\n");
    test->total = 1;
    return true;
}

