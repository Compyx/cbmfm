/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test-lib-base-dxx.c
 * \brief   Unit test for src/lib/base/dxx.c
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
#include "image/d64.h"
#include "base/dir.h"

#include "testcase.h"


/** \brief  Test image: 'Armalyte +7dh 101%/remember'
 */
#define D64_ARMALYTE_FILE   "data/images/d64/armalyte+7dh101%-2004-remember.d64"


static cbmfm_d64_t image;


static bool test_lib_base_dxx_geometry(test_case_t *test);
static bool test_lib_base_dxx_block(test_case_t *test);


static void test_lib_base_dxx_setup(void)
{
    cbmfm_d64_init(&image);
    cbmfm_d64_open(&image, D64_ARMALYTE_FILE);
}


static void test_lib_base_dxx_teardown(void)
{
    cbmfm_d64_cleanup(&image);
}





/** \brief  List of tests for the dxx functions
 */
static test_case_t tests_lib_base_dxx[] = {
    { "geometry", "Dxx image geometry handling",
        test_lib_base_dxx_geometry, 0, 0 },
    { "block", "Dxx image block handling",
        test_lib_base_dxx_block, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base dxx library functions
 */
test_module_t module_lib_base_dxx = {
    "dxx",
    "Dxx library functions",
    tests_lib_base_dxx,
    test_lib_base_dxx_setup,
    test_lib_base_dxx_teardown,
    0, 0
};


/** \brief  Speed zones for D64 images
 *
 * Copied rom d64.c
 */
static const cbmfm_dxx_speedzone_t zones[] = {
    {  1, 17, 21 },
    { 18, 24, 19 },
    { 25, 30, 18 },
    { 31, 40, 17 },
    { -1, -1, -1 }
};


/** \brief  Zone test struct
 */
typedef struct zone_test_s {
    int track;      /**< track# */
    int sector;     /**< sector# */
    int result;     /**< expected result */
} zone_test_t;


/** \brief  list of tests for the zone functions
 */
static const zone_test_t zone_tests[] = {
    /* OK */
    {  1,  0,   0 },
    { 18,  0, 357 },
    { 35, 16, 682 },

    /* fail */
    { -1,  0,  -1 },
    {  1, -1,  -1 },
    { 18, 21,  -1 },

    { -1, -1, -1 }
};


/** \brief  Test geometry functions of dxx.c
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_base_dxx_geometry(test_case_t *test)
{
    int z;

    test->total = 0;

    printf("..... Testing disk zone handling:\n");

    z = 0;
    while (!(zone_tests[z].track == -1 && zone_tests[z].sector == -1)) {
        int t;
        int s;
        int r;
        int result;

        test->total++;

        t = zone_tests[z].track;
        s = zone_tests[z].sector;
        r = zone_tests[z].result;   /* expected result */

        result = cbmfm_dxx_block_number(zones, t, s);
        printf("....... cbmfm_dxx_block_number(%2d, %2d) = %3d -> ",
                t, s, result);
        if (r == result) {
            printf("OK\n");
        } else {
            printf("failed, expected %d\n",r );
            test->failed++;
        }

        z++;
    }

    return true;
}


/** \brief  Test block handling of dxx.c
 *
 * \param[in,out]   test    test case
 *
 * \return  false on failure inside the test code, not on test failure(s)
 */
static bool test_lib_base_dxx_block(test_case_t *test)
{
    cbmfm_block_t block;

    printf("..... Testing block reading (cbmfm_dxx_block_read()):\n");
    test->total = 1;

    cbmfm_block_init(&block);
    printf("....... Reading block (18,0) from test image .. ");
    if (!(cbmfm_dxx_block_read(&block, (cbmfm_dxx_image_t *)(&image), 18, 0))) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK, dumping block data:\n");
        cbmfm_dxx_block_dump(&block);
        cbmfm_dxx_block_cleanup(&block);
    }

    return true;
}
