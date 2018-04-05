/* vim: set et ts=4 sw=4 sts=4 fdm=marker syntax=c.doxygen: */

/** \file   src/tests/test-lib-image-d64.c
 * \brief   Unit test for src/lib/image/d64.c
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


/** \brief  Test image: 'Topaz tools'
 */
#define D64_ARMALYTE_FILE   "data/images/d64/armalyte+7dh101%-2004-remember.d64"

/** \brief  Size of test image 'Topaz tools'
 */
#define D64_ARMALYTE_SIZE   174848


static bool test_lib_image_d64_open(test_case_t *test);
#if 0
static bool test_lib_image_ark_dir(test_case_t *test);
static bool test_lib_image_ark_file(test_case_t *test);
#endif

/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_image_d64[] = {
    { "open", "Opening d64 images",
        test_lib_image_d64_open, 0, 0 },
#if 0
    { "dir", "Directory handling of Ark archives",
        test_lib_image_ark_dir, 0, 0 },
    { "file", "File handling of Ark archives",
        test_lib_image_ark_file, 0, 0 },
#endif
    { NULL, NULL, NULL, 0, 0 }
};


/** \brief  Test module for the base library functions
 */
test_module_t module_lib_image_d64 = {
    "d64",
    "D64 library functions",
    tests_lib_image_d64,
    NULL,
    NULL,
    0, 0
};


/** \brief  Test opening of D64 images
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_d64_open(struct test_case_s *test)
{
    cbmfm_d64_t image;
    bool result;

    test->total = 2;

    cbmfm_d64_init(&image);

    printf("..... calling cbmfm_d64_open(\"%s\" ... ", D64_ARMALYTE_FILE);
    result = cbmfm_d64_open(&image, D64_ARMALYTE_FILE);
    if (!result) {
        /* fatal error*/
        printf("failed: fatal\n");
        return false;
    }

    /* check size */
    printf("OK\n");
    printf("..... expected size: %zu, got size %zu ... ",
            (size_t)D64_ARMALYTE_SIZE, (size_t)(image.size));
    if (image.size != D64_ARMALYTE_SIZE) {
        printf("fail\n");
        test->failed++;
    } else {
        printf("OK\n");
    }

    /* check readonly */
    result = cbmfm_image_get_readonly((cbmfm_image_t *)&image);
    printf("..... checking readonly flag: expected false, got %s ... ",
            result ? "true" : "false");
    if (!result) {
        printf("OK\n");
    } else {
        printf("Failed\n");
        test->failed++;
    }

    /* check dirty */
    result = cbmfm_image_get_dirty((cbmfm_image_t *)&image);
    printf("..... checking dirty flag: expected false, got %s ... ",
            result ? "true" : "false");
    if (!result) {
        printf("OK\n");
    } else {
        printf("Failed\n");
        test->failed++;
    }
#if 0
    printf("..... dumping stat via cbmfm_ark_dump_stats():\n");
    cbmfm_ark_dump_stats(&image);
#endif

    printf("..... calling cbmfm_d64_cleanup()\n");
    cbmfm_d64_cleanup(&image);
    return true;
}


#if 0
/** \brief  Test directory handling of Ark archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_ark_dir(struct test_case_s *test)
{
    cbmfm_image_t image;
    cbmfm_dir_t *dir;
    bool result;

    test->total = 1;

    cbmfm_image_init(&image);

    printf("..... calling cbmfm_ark_open(\"%s\" ... ", ARK_TPZTOOLS_FILE);
    result = cbmfm_ark_open(&image, ARK_TPZTOOLS_FILE);
    if (!result) {
        /* fatal error*/
        printf("failed: fatal\n");
        return false;
    }

    printf("..... dumping stat via cbmfm_ark_dump_stats():\n");
    cbmfm_ark_dump_stats(&image);

    printf("..... dumping directory:\n");
    dir = cbmfm_ark_read_dir(&image, true);
    if (dir == NULL) {
        fprintf(stderr, "failed!\n");
        test->failed++;
    } else {
        cbmfm_dir_dump(dir);
        cbmfm_dir_free(dir);
    }

    printf("..... calling cbmfm_ark_cleanup()\n");
    cbmfm_image_cleanup(&image);
    return true;
}


/** \brief  Test file handling of Ark archives
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 *
 * \todo    allow a prefix for the files, perhaps based on the image name?
 */
static bool test_lib_image_ark_file(struct test_case_s *test)
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

    printf("..... dumping stat via cbmfm_ark_dump_stats():\n");
    cbmfm_ark_dump_stats(&image);

    /* this isn't supposed to fail */
    printf("..... dumping directory:\n");
    dir = cbmfm_ark_read_dir(&image, true);
    if (dir == NULL) {
        fprintf(stderr, "failed!\n");
        return false;
    }
    cbmfm_dir_dump(dir);
    cbmfm_dir_free(dir);

    /* extract file with original PETSCII name */
    printf("..... calling cbmfm_ark_extract_file(image, NULL, 0) .. ");
    if (!cbmfm_ark_extract_file(&image, NULL, 0)) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK\n");
    }

    /* extract file with custom name */
    printf("..... calling cbmfm_ark_extract_file(image, \"dlw-2.4.prg\", 0) .. ");
    if (!cbmfm_ark_extract_file(&image, "dlw-2.4.prg", 0)) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK\n");
    }

    /* extract all files */
    printf("..... calling cbmfm_ark_extract_all() .. ");
    if (!cbmfm_ark_extract_all(&image)) {
        printf("failed\n");
        test->failed++;
    } else {
        printf("OK\n");
    }


    printf("..... calling cbmfm_ark_cleanup()\n");
    cbmfm_image_cleanup(&image);
    return true;
}
#endif
