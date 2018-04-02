/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test-lib-image-ark.c
 * \brief   Unit test for src/lib/image/ark.c
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
#include "image/ark.h"
#include "base/dir.h"

#include "testcase.h"


#define ARK_TPZTOOLS_FILE   "data/images/ark/Tpztools.ark"
#define ARK_TPZTOOLS_SIZE   40744


static bool test_lib_image_ark_open(test_case_t *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_image_ark[] = {
    { "open", "Opening Ark archives", test_lib_image_ark_open, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_image_ark = {
    "ark",
    "Ark library functions",
    tests_lib_image_ark,
    NULL,
    NULL,
    0, 0
};


/** \brief  Test src/lib/base/image/ark.c behaviour
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_ark_open(struct test_case_s *test)
{
    cbmfm_image_t image;
    cbmfm_dir_t *dir;
    bool result;

    test->total = 3;

    cbmfm_image_init(&image);

    printf("..... calling cbmfm_ark_open(\"%s\" ... ", ARK_TPZTOOLS_FILE);
    result = cbmfm_ark_open(&image, ARK_TPZTOOLS_FILE);
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
    printf("..... checking readonly flag: expected true, got %s ... ",
            result ? "true" : "false");
    if (result) {
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

    printf("..... dumping stat via cbmfm_ark_dump_stats():\n");
    cbmfm_ark_dump_stats(&image);

    printf("..... dumping directory:\n");
    dir = cbmfm_ark_read_dir(&image);
    if (dir == NULL) {
        fprintf(stderr, "failed!\n");
        return false;
    }
    cbmfm_dir_dump(dir);
    cbmfm_dir_free(dir);


    printf("... calling cbmfm_ark_cleanup()\n");
    cbmfm_image_cleanup(&image);
    return true;
}
