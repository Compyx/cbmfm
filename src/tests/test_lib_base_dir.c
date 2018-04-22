/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test_lib_base_dir.c
 * \brief   Unit test for src/lib/base/dir.c
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
#include "base/image.h"
#include "base/dxx.h"
#include "base/dir.h"
#include "base/dirent.h"
#include "image/d64.h"
#include "testcase.h"

#include "test_lib_base_dir.h"


/** \brief  Test image: 'Armalyte +7dh 101%/remember'
 */
#define D64_ARMALYTE_FILE   "data/images/d64/armalyte+7dh101%-2004-remember.d64"


/** \brief  Test image object
 *
 * Gets intialized by the module's setup() function and cleaned up by the
 * module's teardown() function
 */
static cbmfm_d64_t image;


static bool test_lib_base_dir_iter(test_case_t *test);


/** \brief  Setup function for the test module
 *
 * Opens the Armalyte test image
 */
static void test_lib_base_dir_setup(void)
{
    cbmfm_d64_init(&image);
    cbmfm_d64_open(&image, D64_ARMALYTE_FILE);
}

/** \brief  Teardown function for the test module
 *
 * Closes the Armalyte test image
 */
static void test_lib_base_dir_teardown(void)
{
    cbmfm_d64_cleanup(&image);
}





/** \brief  List of tests for the directory functions
 */
static test_case_t tests_lib_base_dir[] = {
    { "iter", "Dxx directory iterator handling",
        test_lib_base_dir_iter, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base dxx library functions
 */
test_module_t module_lib_base_dir = {
    "dir",
    "Directory functions",
    tests_lib_base_dir,
    test_lib_base_dir_setup,
    test_lib_base_dir_teardown,
    0, 0
};


/** \brief  Test directory iterator functions of dir.c
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_base_dir_iter(test_case_t *test)
{
    cbmfm_dxx_dir_iter_t iter;
    cbmfm_dirent_t dirent;
    int count;

    test->total = 1;

    printf("..... Initializing dir iter to (18,1) .. ");
    if (!cbmfm_dxx_dir_iter_init(&iter, (cbmfm_dxx_image_t *)&image, 18, 1)) {
        printf("failed -> fatal\n");
        test->failed++;
        return false;
    }
    printf("OK\n");

    /* bad test, I know */
    count = 0;
    do {
        uint8_t *data;

        printf("%2d,%2d, %02x:\n",
                iter.block_iter.curr.track,
                iter.block_iter.curr.sector,
                (unsigned int)iter.entry_offset);

        data = cbmfm_dxx_dir_iter_entry_ptr(&iter);
        cbmfm_hexdump(data, 0, 32);
        cbmfm_d64_dirent_parse(&dirent, data);
        cbmfm_dirent_dump(&dirent);
        putchar('\n');

        count++;

    } while (cbmfm_dxx_dir_iter_next(&iter));
    printf("\n..... got %d entries -> %s\n",
            count, count == 12 ? "OK" : "failed");
    if (count != 12) {
        test->failed++;
    }

    return true;
}
