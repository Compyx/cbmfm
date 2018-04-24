/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test_lib_image_lnx.c
 * \brief   Unit test for src/lib/image/lnx.c
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

#include "lib/base/dir.h"
#include "lib/base/errors.h"
#include "lib/base/file.h"
#include "lib/image/lnx.h"
#include "testcase.h"

#include "test_lib_image_lnx.h"


/** \brief  List of test images
 */
static const char *lnx_images[] = {
    "data/images/lnx/Too Wicked.lnx",
    "data/images/lnx/Phobos.lnx",
    "data/images/lnx/Handjob Lotion.lnx",
    "data/images/lnx/party-demo.lnx",
    "data/images/lnx/GC97.LNX",
    NULL
};


static bool test_lib_image_lnx_open(test_case_t *test);
static bool test_lib_image_lnx_dir(test_case_t *test);
static bool test_lib_image_lnx_file(test_case_t *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_image_lnx[] = {
    { "open", "Opening Lynx archives",
        test_lib_image_lnx_open, 0, 0 },
    { "dir", "Directory handling of :ynx archives",
        test_lib_image_lnx_dir, 0, 0 },
    { "file", "File handling of Lynx archives",
        test_lib_image_lnx_file, 0, 0 },
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_image_lnx = {
    "lnx",
    "Lynx library functions",
    tests_lib_image_lnx,
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
static bool test_lib_image_lnx_open(test_case_t *test)
{
    cbmfm_lnx_t image;
    int i;

    test->total = 0;

    for (i = 0; lnx_images[i] != NULL; i++) {
        test->total++;
        printf("\n..... opening '%s' .. ", lnx_images[i]);
        cbmfm_lnx_init(&image);
        if (!cbmfm_lnx_open(&image, lnx_images[i])) {
            printf("failed\n");
            cbmfm_perror("test_lib_image_lnx_open");
            test->failed++;
        } else {
            printf("OK, dumping header data:\n");
            cbmfm_lnx_dump(&image);

            cbmfm_lnx_cleanup(&image);
        }
    }

    /* TODO: also add some non-Lynx images to the list of images to test */
    printf("..... testing Lynx image detection:\n");
    for (i = 0; lnx_images[i] != NULL; i++) {
        bool result;

        test->total++;

        result = cbmfm_is_lnx(lnx_images[i]);
        printf("\n....... calling cbmfm_is_lnx('%s') = %s -> %s",
                lnx_images[i], result ? "true" : "false",
                result ? "OK" : "false");
        if (!result) {
            test->failed++;
        }
    }


    cbmfm_lnx_init(&image);

    return true;
}


/** \brief  Test reading directories of Lynx archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_lnx_dir(test_case_t *test)
{
    cbmfm_lnx_t image;
    cbmfm_dir_t *dir;
    int i;

    test->total = 5;

    for (i = 0; lnx_images[i] != NULL; i++) {
        printf("\n..... opening '%s' .. ", lnx_images[i]);
        cbmfm_lnx_init(&image);
        if (!cbmfm_lnx_open(&image, lnx_images[i])) {
            printf("failed\n");
            cbmfm_perror("test_lib_image_lnx_open");
            return false;
        } else {
            printf("OK, reading directory ... ");
            dir = cbmfm_lnx_dir_read(&image);
            if (dir == NULL) {
                printf("failed\n");
                test->failed++;
            } else {
                printf("OK, dumping dir:\n");
                cbmfm_dir_dump(dir);
                cbmfm_dir_free(dir);
            }

            cbmfm_lnx_cleanup(&image);
        }
    }

    cbmfm_lnx_init(&image);

    return true;
}



/** \brief  Test file handling of Lynx archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_lnx_file(test_case_t *test)
{
    cbmfm_lnx_t image;
    cbmfm_dir_t *dir;
    cbmfm_file_t file;

    test->total = 2;

    printf("\n..... opening '%s' .. ", lnx_images[0]);
    cbmfm_lnx_init(&image);
    if (!cbmfm_lnx_open(&image, lnx_images[0])) {
        printf("failed\n");
        cbmfm_perror("test_lib_image_lnx_open");
        return false;
    }
    printf("OK, reading directory ... ");
    dir = cbmfm_lnx_dir_read(&image);
    if (dir == NULL) {
        printf("failed\n");
        return false;
    }
    printf("OK, dumping dir:\n");
    cbmfm_dir_dump(dir);

    printf("..... reading file at index 0\n");
    if (!cbmfm_lnx_file_read(dir, &file, 0)) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK, writing to host.\n");
        cbmfm_file_dump(&file);
        if (!cbmfm_file_write_host(&file, NULL)) {
            cbmfm_file_cleanup(&file);
            cbmfm_dir_free(dir);
            return false;
        }
        cbmfm_file_cleanup(&file);
    }

    cbmfm_lnx_cleanup(&image);
    cbmfm_dir_free(dir);

    printf("\n..... opening '%s' .. ", lnx_images[2]);
    cbmfm_lnx_init(&image);
    if (!cbmfm_lnx_open(&image, lnx_images[2])) {
        printf("failed\n");
        cbmfm_perror("test_lib_image_lnx_open");
        return false;
    }
    printf("OK, reading directory ... ");
    dir = cbmfm_lnx_dir_read(&image);
    if (dir == NULL) {
        printf("failed\n");
        return false;
    }
    printf("OK, dumping dir:\n");
    cbmfm_dir_dump(dir);

    printf("..... reading file at index 4\n");
    if (!cbmfm_lnx_file_read(dir, &file, 4)) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK, writing to host.\n");
        cbmfm_file_dump(&file);
        if (!cbmfm_file_write_host(&file, NULL)) {
            cbmfm_file_cleanup(&file);
            cbmfm_dir_free(dir);
            return false;
        }
        cbmfm_file_cleanup(&file);
    }


    cbmfm_dir_free(dir);
    cbmfm_lnx_cleanup(&image);

    return true;
}
