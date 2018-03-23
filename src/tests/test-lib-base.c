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

#include "lib.h"

#include "testcase.h"


#define ARK_TPZTOOLS_FILE   "data/images/ark/Tpztools.ark"
#define ARK_TPZTOOLS_SIZE   40744


static bool test_lib_base_io(struct test_case_s *test);
static bool test_lib_base_image(struct test_case_s *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_base[] = {
    { "io", "I/O handling", test_lib_base_io, 0, 0 },
    { "image", "Basic image handling", test_lib_base_image, 0, 0 },
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
    test->total = 1;
    printf("..... TODO: actually implement tests here\n");
    return true;
}


static bool test_lib_base_image(struct test_case_s *test)
{
    cbmfm_image_t image;
    bool result;

    test->total = 3;

    cbmfm_image_init(&image);
    printf("..... calling cbmfm_image_read_data(\"%s\" ... ", ARK_TPZTOOLS_FILE);
    result = cbmfm_image_read_data(&image, ARK_TPZTOOLS_FILE);
    if (!result) {
        /* fatal error*/
        printf("failed: fatal\n");
        return false;
    }

    /* check size */
    printf("OK\n");
    printf("..... expected size: %zu, got size %zu ... ",
            (size_t)ARK_TPZTOOLS_SIZE, (size_t)(image.size));
    if (image.size != ARK_TPZTOOLS_SIZE) {
        printf("fail\n");
        test->failed++;
    } else {
        printf("OK\n");
    }

    /* check readonly */
    result = cbmfm_image_get_readonly(&image);
    printf("..... checking readonly flag: expected false, got %s ... ",
            result ? "true" : "false");
    if (!result) {
        printf("OK\n");
    } else {
        printf("Failed\n");
        test->failed++;
    }

    /* check dirty */
    result = cbmfm_image_get_dirty(&image);
    printf("..... checking dirty flags: expected false, got %s ... ",
            result ? "true" : "false");
    if (!result) {
        printf("OK\n");
    } else {
        printf("Failed\n");
        test->failed++;
    }


    printf("... calling cbmfm_image_cleanup()\n");
    cbmfm_image_cleanup(&image);
    return true;
}
