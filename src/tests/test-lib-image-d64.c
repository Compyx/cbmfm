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
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "lib.h"
#include "image/d64.h"
#include "base/dir.h"

#include "testcase.h"


/** \brief  Test image: 'Armalyte +7dh 101%/remember'
 */
#define D64_ARMALYTE_FILE   "data/images/d64/armalyte+7dh101%-2004-remember.d64"

/** \brief  Size of test image 'Armalyte'
 */
#define D64_ARMALYTE_SIZE   174848


/** \brief  Disk name of 'Armalyte' in ASCII
 */
#define D64_ARMALYTE_DISK_NAME_ASC  "once upon a time"


/** \brief  Disk ID (extended) of 'Armalyte' in ASCII
 *
 * Extended disk ID, meaning 5 bytes (ID + unused byte + DOStype)
 */
#define D64_ARMALYTE_DISK_ID_ASC  "-rem-"




static bool test_lib_image_d64_open(test_case_t *test);
static bool test_lib_image_d64_bam(test_case_t *test);


/** \brief  List of tests for the base library functions
 */
static test_case_t tests_lib_image_d64[] = {
    { "open", "Opening D64 images",
        test_lib_image_d64_open, 0, 0 },
    { "bam", "BAM handling of D64 images",
        test_lib_image_d64_bam, 0, 0 },
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
static bool test_lib_image_d64_open(test_case_t *test)
{
    cbmfm_d64_t image;
    bool result;

    test->total = 3;

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
    }
    printf("OK\n");
    printf("....... track count = %d, error bytes = %s\n",
            image.track_max, image.errors ? "yes" : "no");

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


/** \brief  Test BAM handling of D64 images
 *
 * \param[in,out]   test    test object
 *
 * \return  bool
 */
static bool test_lib_image_d64_bam(test_case_t *test)
{
    cbmfm_d64_t image;
    bool result;
    uint8_t *bam;
    long offset;
    char disk_name[CBMFM_CBMDOS_FILE_NAME_LEN + 1];
    char disk_id[CBMFM_CBMDOS_DISK_ID_LEN_EXT + 1];

    test->total = 3;

    cbmfm_d64_init(&image);

    printf("..... calling cbmfm_d64_open(\"%s\" ... ", D64_ARMALYTE_FILE);
    result = cbmfm_d64_open(&image, D64_ARMALYTE_FILE);
    if (!result) {
        /* fatal error*/
        printf("failed: fatal\n");
        return false;
    }
    printf("OK\n");

    bam = cbmfm_d64_bam_ptr(&image);
    offset = bam - image.data;
    printf("..... calling c64_d64_bam_ptr() = %p (offset = $%lx) -> %s\n",
            bam, offset, offset == 0x16500 ? "OK" : "failed");
    if (offset != 0x16500) {
        test->failed++;
        printf("BAM ptr test failed, all others test will fail too, exiting\n");
        cbmfm_d64_cleanup(&image);
        return false;
    }

    printf("..... calling cbmfm_get_disk_name_asc()..\n");
    cbmfm_d64_get_disk_name_asc(&image, disk_name);
    result = strcmp(disk_name, D64_ARMALYTE_DISK_NAME_ASC) == 0;
    printf("....... disk name = '%s' -> %s\n", disk_name,
            result ? "OK" : "failed");
    if (!result) {
        test->failed++;
    }

    printf("..... calling cbmfm_get_disk_id_asc()..\n");
    cbmfm_d64_get_disk_id_asc(&image, disk_id);
    result = strcmp(disk_id, D64_ARMALYTE_DISK_ID_ASC) == 0;
    printf("....... disk id = '%s' -> %s\n", disk_id,
            result ? "OK" : "failed");
    if (!result) {
        test->failed++;
    }


    cbmfm_d64_cleanup(&image);
    return true;
}
