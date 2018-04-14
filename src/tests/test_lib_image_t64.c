/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test_lib_image_t64.c
 * \brief   Unit test for src/lib/image/t64.c
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
#include "image/t64.h"
#include "base/dir.h"
#include "testcase.h"

#include "test_lib_image_t64.h"


/** \brief  List of test images
 */
static const char *t64_images[] = {
    "data/images/t64/c64sfreeze-2.52.t64",
    "data/images/t64/compunet-OK.t64",
    "data/images/t64/kikstart_iii-wrong-end-address.t64",
    "data/images/t64/pallino-padded.t64",
    NULL
};


static bool test_lib_image_t64_open(test_case_t *test);
static bool test_lib_image_t64_dir(test_case_t *test);
static bool test_lib_image_t64_file(test_case_t *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_image_t64[] = {
    { "open", "Opening T64 archives",
        test_lib_image_t64_open, 0, 0 },
    { "dir", "Directory handling of T64 archives",
        test_lib_image_t64_dir, 0, 0 },
    { "file", "File handling of T64 archives",
        test_lib_image_t64_file, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_image_t64 = {
    "t64",
    "T64 library functions",
    tests_lib_image_t64,
    NULL,
    NULL,
    0, 0
};


/** \brief  Test opening of T64 archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_t64_open(test_case_t *test)
{
    cbmfm_t64_t image;
    int i;

    test->total = 4;

    for (i = 0; t64_images[i] != NULL; i++) {
        printf("..... opening '%s' .. ", t64_images[i]);
        cbmfm_t64_init(&image);
        if (!cbmfm_t64_open(&image, t64_images[i])) {
            printf("failed\n");
            test->failed++;
        } else {
            printf("OK, dumping header data:\n");
            cbmfm_t64_dump_header(&image);

            cbmfm_t64_cleanup(&image);
        }
    }

    cbmfm_t64_init(&image);

    return true;
}


/** \brief  Test directory handling of T64 archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_t64_dir(test_case_t *test)
{
    cbmfm_t64_t image;
    cbmfm_dir_t *dir;
    int i;

    test->total = 8;

    for (i = 0; t64_images[i] != NULL; i++) {
        printf("..... opening '%s' .. ", t64_images[i]);
        cbmfm_t64_init(&image);
        if (!cbmfm_t64_open(&image, t64_images[i])) {
            printf("failed\n");
            test->failed++;
        } else {

            cbmfm_dirent_t dirent;

            printf("OK, dumping header data:\n");
            cbmfm_t64_dump_header(&image);

            printf("..... parsing dirent #0 .. ");
            if (!cbmfm_t64_dirent_parse(&image, &dirent, 0)) {
                printf("failed: ");
                fflush(stdout);
                cbmfm_perror(NULL);
                test->failed++;
            } else {
                printf("OK, dumping dirent:\n");
                cbmfm_dirent_dump(&dirent);
            }
            putchar('\n');

            printf("..... calling cbmfm_t64_read_dir() .. ");
            fflush(stdout);
            dir = cbmfm_t64_read_dir(&image);
            if (dir == NULL) {
                printf("failed\n");
                test->failed++;
            } else {
                printf("OK, dumping dir:\n");
                cbmfm_dir_dump(dir);
                cbmfm_dir_free(dir);
            }

            cbmfm_t64_cleanup(&image);
        }
    }

   return true;
}


/** \brief  Test file handling of T64 archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_t64_file(test_case_t *test)
{
    cbmfm_t64_t image;
    cbmfm_dir_t *dir;
    int i;

    test->total = 8;

    for (i = 0; t64_images[i] != NULL; i++) {
        printf("..... opening '%s' .. ", t64_images[i]);
        cbmfm_t64_init(&image);
        if (!cbmfm_t64_open(&image, t64_images[i])) {
            printf("failed\n");
            test->failed++;
        } else {

            printf("..... calling cbmfm_t64_read_dir() .. ");
            fflush(stdout);
            dir = cbmfm_t64_read_dir(&image);
            if (dir == NULL) {
                printf("failed\n");
                cbmfm_t64_cleanup(&image);
                return false;   /* fatal error */
            } else {
                printf("OK, dumping dir:\n");
                cbmfm_dir_dump(dir);

                printf("....... saving file #0 using PETSCII-converted name .. ");
                if (!cbmfm_t64_extract_file(dir, 0, NULL)) {
                    test->failed++;
                    printf("failed\n");
                } else {
                    printf("OK\n");
                }

                cbmfm_dir_free(dir);

            }

            cbmfm_t64_cleanup(&image);
        }
    }

   return true;
}




